// Emulates the few Zen 2 instructions in the game's code that a desktop CPU may lack (docs/cpu_features.md).
//
// The executable is built for the console's AMD Zen 2 and uses some of its instructions without asking CPUID:
// SSE4a INSERTQ/EXTRQ, CLZERO and MONITORX/MWAITX (AMD only) and CLWB (Intel since Ice Lake). On a host without
// one, the CPU raises #UD (STATUS_ILLEGAL_INSTRUCTION) at that instruction. A vectored handler recognises these
// encodings at the faulting RIP, does the same work on the thread's CONTEXT and continues after the instruction.
// On a host that has the feature the CPU runs the instruction itself and the handler never sees it.
//
// What each one does, from the AMD64 Architecture Programmer's Manual (vol. 3 for CLZERO, MONITORX, MWAITX; vol. 4
// for INSERTQ, EXTRQ) and the Intel SDM vol. 2 for CLWB:
//   INSERTQ xmm1, xmm2, len, idx   F2 0F 78 /r ib ib   insert the low `len` bits of xmm2[63:0] into xmm1[63:0] at
//                                                       bit `idx` (len 0 means 64)
//   INSERTQ xmm1, xmm2             F2 0F 79 /r         same, len = xmm2[69:64], idx = xmm2[77:72]
//   EXTRQ   xmm1, len, idx         66 0F 78 /0 ib ib   xmm1[63:0] = (xmm1[63:0] >> idx), `len` bits, zero-extended
//   EXTRQ   xmm1, xmm2             66 0F 79 /r         same, len = xmm2[5:0], idx = xmm2[13:8]
//   CLZERO                         0F 01 FC            zero the 64-byte cache line that rAX points into; a line
//                                                       it cannot write raises that write fault, as the CPU does
//   MONITORX                       0F 01 FA            arm an address monitor: a hint, nothing to do here
//   MWAITX                         0F 01 FB            wait for the monitor or a timer; returning at once is one
//                                                       of the exits the manual allows, so it is a pause
//   CLWB m8                        66 0F AE /6 (mem)   write a line back, keeping it cached: ordinary write-back
//                                                       memory is coherent, so there is nothing to do here
// The APM leaves xmm1[127:64] undefined after INSERTQ and EXTRQ; a Zen 3 zeroes it in all four forms (measured in
// #7111's review), so the emulation does too. A field with len + idx > 64 is undefined as well; the emulation keeps
// the bits that fit, which matched that Zen 3 on every len/idx pair.
#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace rt::cpu_emulation {

enum class Op { none, insertq_imm, insertq_reg, extrq_imm, extrq_reg, clzero, monitorx, mwaitx, clwb };

struct Decoded {
    Op op = Op::none;
    std::size_t length = 0;  // bytes, prefixes included
    int dst = 0;             // xmm register (INSERTQ, EXTRQ)
    int src = 0;             // xmm register (the register forms)
    uint8_t len = 0;         // field length and index of the immediate forms
    uint8_t idx = 0;
    bool addr32 = false;     // a 67 prefix: CLZERO uses EAX
};

// One of the encodings above at `code` (`avail` readable bytes), or Op::none.
inline Decoded decode(const uint8_t* code, std::size_t avail) {
    Decoded d;
    std::size_t i = 0;
    if (avail > 15) avail = 15;  // an x86 instruction is at most 15 bytes; a longer one is #UD on the CPU too
    bool p66 = false, pf2 = false, pf3 = false;
    uint8_t rex = 0;
    // Legacy prefixes and REX bytes in any order; only a REX right before the opcode counts, an earlier one is
    // ignored (as AMD CPUs do: `48 66 0F 01 FC` and `48 48 0F 01 FC` are CLZERO). LOCK (F0) ends the scan: #UD.
    for (; i < avail && i < 14; ++i) {
        const uint8_t b = code[i];
        if ((b & 0xF0) == 0x40) { rex = b; continue; }
        if (b == 0x66) p66 = true;
        else if (b == 0xF2) pf2 = true;
        else if (b == 0xF3) pf3 = true;
        else if (b == 0x67) d.addr32 = true;
        else if (b == 0x2E || b == 0x3E || b == 0x26 || b == 0x36 || b == 0x64 || b == 0x65) {}
        else break;
        rex = 0;
    }
    if (i + 2 > avail || code[i] != 0x0F) return {};
    const uint8_t op = code[i + 1];
    i += 2;
    if (op == 0x01) {  // CLZERO, MONITORX, MWAITX
        if (i >= avail) return {};
        const uint8_t m = code[i++];
        // AMD CPUs run CLZERO with 66, F2 or F3 in front (measured on Zen 3 in #7055's reviews). For FA/FB an F3
        // selects another instruction (F3 0F 01 FA is MCOMMIT), and no 66 or F2 form is measured, so MONITORX and
        // MWAITX are only taken without those prefixes (the title's own sites carry none).
        if ((p66 || pf2 || pf3) && m != 0xFC) return {};
        d.op = m == 0xFC ? Op::clzero : m == 0xFA ? Op::monitorx : m == 0xFB ? Op::mwaitx : Op::none;
        d.length = i;
        return d.op == Op::none ? Decoded{} : d;
    }
    if (i >= avail) return {};
    const uint8_t modrm = code[i++];
    const int mod = modrm >> 6, reg = ((modrm >> 3) & 7) | ((rex & 4) << 1), rm = (modrm & 7) | ((rex & 1) << 3);
    if (op == 0x78 || op == 0x79) {  // SSE4a; F2 is INSERTQ, 66 is EXTRQ; both need a register operand
        if (mod != 3 || pf3 || p66 == pf2) return {};
        const bool imm = op == 0x78;
        if (imm) {
            if (i + 2 > avail) return {};
            if (p66 && (modrm & 0x38)) return {};  // EXTRQ's immediate form is /0
            d.len = code[i] & 0x3F;
            d.idx = code[i + 1] & 0x3F;
            i += 2;
        }
        d.op = pf2 ? (imm ? Op::insertq_imm : Op::insertq_reg) : (imm ? Op::extrq_imm : Op::extrq_reg);
        if (p66 && imm) d.dst = rm;  // EXTRQ xmm1, imm, imm names xmm1 in r/m
        else d.dst = reg, d.src = rm;
        d.length = i;
        return d;
    }
    if (op == 0xAE && p66 && !pf2 && !pf3 && mod != 3 && ((modrm >> 3) & 7) == 6) {  // CLWB m8
        if ((modrm & 7) == 4) {  // SIB
            if (i >= avail) return {};
            const uint8_t sib = code[i++];
            if (mod == 0 && (sib & 7) == 5) i += 4;
        } else if (mod == 0 && (modrm & 7) == 5) {
            i += 4;  // RIP-relative
        }
        i += mod == 1 ? 1 : mod == 2 ? 4 : 0;
        if (i > avail) return {};
        d.op = Op::clwb;
        d.length = i;
        return d;
    }
    return {};
}

inline uint64_t field_mask(unsigned len) { return len >= 64 ? ~uint64_t(0) : (uint64_t(1) << len) - 1; }

// INSERTQ on the low quadword: `len` (0 means 64) bits of `src` placed at bit `idx` of `dst`.
inline uint64_t insert_bits(uint64_t dst, uint64_t src, unsigned len, unsigned idx) {
    len &= 63;
    idx &= 63;
    const uint64_t mask = field_mask(len ? len : 64) << idx;
    return (dst & ~mask) | ((src << idx) & mask);
}

// EXTRQ on the low quadword: `len` (0 means 64) bits of `src` from bit `idx`, zero-extended.
inline uint64_t extract_bits(uint64_t src, unsigned len, unsigned idx) {
    len &= 63;
    idx &= 63;
    return (src >> idx) & field_mask(len ? len : 64);
}

}  // namespace rt::cpu_emulation

#ifdef _WIN32
#include <windows.h>
#include <intrin.h>
#include <psapi.h>

#include <atomic>
#include <cstdio>

namespace rt::cpu_emulation {

struct Host {
    bool sse4a = false, monitorx = false, clzero = false, clwb = false;
};

inline Host detect_host() {
    Host h;
    int r[4];
    __cpuid(r, 0);
    const int max_leaf = r[0];
    if (max_leaf >= 7) {
        __cpuidex(r, 7, 0);
        h.clwb = (r[1] >> 24) & 1;  // EBX bit 24
    }
    __cpuid(r, 0x80000000);
    const unsigned max_ext = unsigned(r[0]);
    if (max_ext >= 0x80000001) {
        __cpuid(r, 0x80000001);
        h.sse4a = (r[2] >> 6) & 1;      // ECX bit 6
        h.monitorx = (r[2] >> 29) & 1;  // ECX bit 29
    }
    if (max_ext >= 0x80000008) {
        __cpuid(r, 0x80000008);
        h.clzero = r[1] & 1;  // EBX bit 0
    }
    return h;
}

inline const char* name(Op op) {
    switch (op) {
    case Op::insertq_imm: case Op::insertq_reg: return "INSERTQ (SSE4a)";
    case Op::extrq_imm: case Op::extrq_reg: return "EXTRQ (SSE4a)";
    case Op::clzero: return "CLZERO";
    case Op::monitorx: return "MONITORX";
    case Op::mwaitx: return "MWAITX";
    case Op::clwb: return "CLWB";
    default: return "none";
    }
}

// How many times each instruction was emulated; the two forms of INSERTQ and of EXTRQ share one count.
inline std::atomic<uint64_t>& count(Op op) {
    static std::atomic<uint64_t> counts[9];
    if (op == Op::insertq_reg) op = Op::insertq_imm;
    if (op == Op::extrq_reg) op = Op::extrq_imm;
    return counts[int(op)];
}

// 0 when the page holding `addr` can be written; otherwise the exception a write raises there. A guard page loses
// its guard on that first touch and reports STATUS_GUARD_PAGE_VIOLATION, as the system does for a real access.
inline DWORD writable(uint64_t addr) {
    // A resident page answers from the working set in constant time, while VirtualQuery's cost grows with the rest
    // of the region (it sizes the run of pages with the same attributes): up to 0.7 ms in a large heap, per line
    // (#7111 review). A page that is not resident, or not plainly writable, takes the VirtualQuery path below.
    PSAPI_WORKING_SET_EX_INFORMATION ws{};
    ws.VirtualAddress = reinterpret_cast<void*>(addr & ~uint64_t(0xFFF));
    if (QueryWorkingSetEx(GetCurrentProcess(), &ws, sizeof ws) && ws.VirtualAttributes.Valid) {
        const DWORD wp = DWORD(ws.VirtualAttributes.Win32Protection);
        const DWORD p = wp & 0xFF;
        if (!(wp & PAGE_GUARD) && (p == PAGE_READWRITE || p == PAGE_WRITECOPY || p == PAGE_EXECUTE_READWRITE ||
                                   p == PAGE_EXECUTE_WRITECOPY))
            return 0;
    }
    MEMORY_BASIC_INFORMATION mbi;
    if (!VirtualQuery(reinterpret_cast<void*>(addr), &mbi, sizeof mbi) || mbi.State != MEM_COMMIT)
        return DWORD(STATUS_ACCESS_VIOLATION);
    if (mbi.Protect & PAGE_GUARD) {
        DWORD old = 0;
        VirtualProtect(reinterpret_cast<void*>(addr & ~uint64_t(0xFFF)), 1, mbi.Protect & ~DWORD(PAGE_GUARD), &old);
        return DWORD(STATUS_GUARD_PAGE_VIOLATION);
    }
    const DWORD p = mbi.Protect & 0xFF;
    const bool ok = p == PAGE_READWRITE || p == PAGE_WRITECOPY || p == PAGE_EXECUTE_READWRITE ||
                    p == PAGE_EXECUTE_WRITECOPY;
    return ok ? 0 : DWORD(STATUS_ACCESS_VIOLATION);
}

// How many of the `want` bytes at `addr` can be read, by page protection (no fault is taken to find out).
inline std::size_t readable(uint64_t addr, std::size_t want) {
    std::size_t got = 0;
    while (got < want) {
        MEMORY_BASIC_INFORMATION mbi;
        if (!VirtualQuery(reinterpret_cast<void*>(addr + got), &mbi, sizeof mbi) || mbi.State != MEM_COMMIT ||
            (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) || !mbi.Protect)
            break;
        const uint64_t end = reinterpret_cast<uint64_t>(mbi.BaseAddress) + mbi.RegionSize;
        got += std::size_t(end - (addr + got)) < want - got ? std::size_t(end - (addr + got)) : want - got;
    }
    return got;
}

inline uint64_t& lo(CONTEXT* c, int r) { return c->FltSave.XmmRegisters[r].Low; }
inline uint64_t hi(const CONTEXT* c, int r) { return uint64_t(c->FltSave.XmmRegisters[r].High); }

// Apply one decoded instruction to `c` and step past it. False when the line CLZERO zeroes cannot be written; then
// `fault`, when given, receives the exception the write raised (STATUS_ACCESS_VIOLATION with the write flag and the
// line's address, or STATUS_GUARD_PAGE_VIOLATION, as the real instruction would raise) and `c` is unchanged.
inline bool apply(const Decoded& d, CONTEXT* c, EXCEPTION_RECORD* fault = nullptr) {
    switch (d.op) {
    case Op::insertq_imm:
        lo(c, d.dst) = insert_bits(lo(c, d.dst), lo(c, d.src), d.len, d.idx);
        c->FltSave.XmmRegisters[d.dst].High = 0;
        break;
    case Op::insertq_reg: {
        const uint64_t ctl = hi(c, d.src);
        lo(c, d.dst) = insert_bits(lo(c, d.dst), lo(c, d.src), unsigned(ctl) & 63, unsigned(ctl >> 8) & 63);
        c->FltSave.XmmRegisters[d.dst].High = 0;
        break;
    }
    case Op::extrq_imm:
        lo(c, d.dst) = extract_bits(lo(c, d.dst), d.len, d.idx);
        c->FltSave.XmmRegisters[d.dst].High = 0;
        break;
    case Op::extrq_reg: {
        const uint64_t ctl = lo(c, d.src);
        lo(c, d.dst) = extract_bits(lo(c, d.dst), unsigned(ctl) & 63, unsigned(ctl >> 8) & 63);
        c->FltSave.XmmRegisters[d.dst].High = 0;
        break;
    }
    case Op::clzero: {
        const uint64_t addr = (d.addr32 ? (c->Rax & 0xFFFFFFFFu) : c->Rax) & ~uint64_t(63);
        // The line's page is checked first instead of catching a fault from the write: a compiler may turn the
        // write into inline stores that a local __try does not cover (clang-cl without /EHa). A 64-byte aligned
        // line never spans two pages, so one query answers for all of it.
        const DWORD code = writable(addr);
        if (code) {
            if (fault) {
                std::memset(fault, 0, sizeof *fault);
                fault->ExceptionCode = code;
                fault->NumberParameters = 2;
                fault->ExceptionInformation[0] = 1;  // a write
                fault->ExceptionInformation[1] = ULONG_PTR(addr);
            }
            return false;
        }
        std::memset(reinterpret_cast<void*>(addr), 0, 64);
        break;
    }
    case Op::mwaitx: YieldProcessor(); break;
    case Op::monitorx: case Op::clwb: break;
    default: return false;
    }
    c->Rip += d.length;
    return true;
}

inline LONG CALLBACK handler(EXCEPTION_POINTERS* info) {
    if (info->ExceptionRecord->ExceptionCode != STATUS_ILLEGAL_INSTRUCTION) return EXCEPTION_CONTINUE_SEARCH;
    CONTEXT* c = info->ContextRecord;
    uint8_t code[15] = {};
    // The CPU fetched this instruction from RIP's page, so only bytes past that page need a (slow) query; the
    // instruction may end at the page boundary.
    const std::size_t in_page = std::size_t(0x1000 - (c->Rip & 0xFFF));
    const std::size_t avail =
        in_page >= sizeof code ? sizeof code : in_page + readable(c->Rip + in_page, sizeof code - in_page);
    std::memcpy(code, reinterpret_cast<const void*>(c->Rip), avail);
    const Decoded d = decode(code, avail);
    if (d.op == Op::none) return EXCEPTION_CONTINUE_SEARCH;
    EXCEPTION_RECORD fault;
    if (!apply(d, c, &fault)) {
        // CLZERO on a line it cannot write: the CPU would raise that fault at this instruction, so the next handlers
        // see it instead of the #UD (RIP still points at the instruction).
        EXCEPTION_RECORD* r = info->ExceptionRecord;
        r->ExceptionCode = fault.ExceptionCode;
        r->ExceptionFlags = 0;
        r->NumberParameters = fault.NumberParameters;
        r->ExceptionInformation[0] = fault.ExceptionInformation[0];
        r->ExceptionInformation[1] = fault.ExceptionInformation[1];
        return EXCEPTION_CONTINUE_SEARCH;
    }
    if (count(d.op).fetch_add(1, std::memory_order_relaxed) == 0)
        std::fprintf(stderr, "[rt] cpu: this CPU has no %s; emulating it (first at %p)\n", name(d.op),
                     reinterpret_cast<void*>(c->Rip - d.length));
    return EXCEPTION_CONTINUE_EXECUTION;
}

// Installs the handler first in the vectored chain, once. Returns false if Windows refused it.
inline bool install() {
    static void* const cookie = AddVectoredExceptionHandler(1, &handler);
    return cookie != nullptr;
}

}  // namespace rt::cpu_emulation
#endif
