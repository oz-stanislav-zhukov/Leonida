// The Zen 2 instruction emulation (src/cpu_emulation.h, docs/cpu_features.md): the decoder on hand-written
// encodings, the bit-field helpers, `apply` on a synthetic CONTEXT, and then each instruction run for real from an
// executable buffer with the handler installed. On a host that has the feature the CPU runs it and the results
// must be the same; on one that lacks it (an Intel CPU for SSE4a, CLZERO and MONITORX) the handler does the work.
// The test prints which path each instruction took. No game files.
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include <vector>

#include "cpu_emulation.h"

namespace ce = rt::cpu_emulation;

namespace {
int failures = 0;

void check(bool ok, const char* what) {
    if (!ok) {
        std::fprintf(stderr, "FAIL %s\n", what);
        ++failures;
    }
}

ce::Decoded dec(std::initializer_list<uint8_t> bytes) {
    std::vector<uint8_t> v(bytes);
    return ce::decode(v.data(), v.size());
}

void test_decode() {
    ce::Decoded d = dec({0xF2, 0x0F, 0x78, 0xC1, 0x10, 0x08});  // insertq xmm0, xmm1, 16, 8
    check(d.op == ce::Op::insertq_imm && d.length == 6 && d.dst == 0 && d.src == 1 && d.len == 16 && d.idx == 8,
          "insertq imm");
    d = dec({0xF2, 0x45, 0x0F, 0x78, 0xD3, 0x05, 0x3B});  // insertq xmm10, xmm11, 5, 59
    check(d.op == ce::Op::insertq_imm && d.length == 7 && d.dst == 10 && d.src == 11 && d.len == 5 && d.idx == 59,
          "insertq imm with REX");
    d = dec({0xF2, 0x0F, 0x79, 0xCA});  // insertq xmm1, xmm2
    check(d.op == ce::Op::insertq_reg && d.length == 4 && d.dst == 1 && d.src == 2, "insertq reg");
    d = dec({0x66, 0x0F, 0x78, 0xC3, 0x08, 0x04});  // extrq xmm3, 8, 4
    check(d.op == ce::Op::extrq_imm && d.length == 6 && d.dst == 3 && d.len == 8 && d.idx == 4, "extrq imm");
    d = dec({0x66, 0x41, 0x0F, 0x78, 0xC7, 0x00, 0x00});  // extrq xmm15, 0, 0
    check(d.op == ce::Op::extrq_imm && d.dst == 15 && d.length == 7, "extrq imm with REX.B");
    d = dec({0x66, 0x0F, 0x79, 0xE9});  // extrq xmm5, xmm1
    check(d.op == ce::Op::extrq_reg && d.dst == 5 && d.src == 1 && d.length == 4, "extrq reg");
    check(dec({0x66, 0x0F, 0x78, 0xC8, 0x08, 0x04}).op == ce::Op::none, "extrq imm needs /0");
    check(dec({0xF2, 0x0F, 0x78, 0x01, 0x08, 0x04}).op == ce::Op::none, "insertq needs a register operand");
    check(dec({0x0F, 0x78, 0xC1}).op == ce::Op::none, "0F 78 without F2/66 is VMREAD, not SSE4a");
    check(dec({0xF3, 0x0F, 0x78, 0xC1, 1, 1}).op == ce::Op::none, "F3 0F 78 is not SSE4a");

    d = dec({0x0F, 0x01, 0xFC});
    check(d.op == ce::Op::clzero && d.length == 3 && !d.addr32, "clzero");
    d = dec({0x67, 0x0F, 0x01, 0xFC});
    check(d.op == ce::Op::clzero && d.length == 4 && d.addr32, "clzero with 67");
    check(dec({0x0F, 0x01, 0xFA}).op == ce::Op::monitorx, "monitorx");
    check(dec({0x0F, 0x01, 0xFB}).op == ce::Op::mwaitx, "mwaitx");
    check(dec({0x0F, 0x01, 0xF9}).op == ce::Op::none, "rdtscp is not handled");
    check(dec({0xF3, 0x0F, 0x01, 0xFA}).op == ce::Op::none, "F3 0F 01 FA is MCOMMIT, not monitorx");
    for (uint8_t pfx : {uint8_t(0x66), uint8_t(0xF2), uint8_t(0xF3), uint8_t(0x48)}) {
        d = dec({pfx, 0x0F, 0x01, 0xFC});
        check(d.op == ce::Op::clzero && d.length == 4, "clzero with 66, F2, F3 or REX (native on Zen 3)");
    }
    d = dec({0x48, 0x66, 0x0F, 0x01, 0xFC});
    check(d.op == ce::Op::clzero && d.length == 5, "clzero with REX before 66 (the REX is ignored)");
    d = dec({0x48, 0x48, 0x0F, 0x01, 0xFC});
    check(d.op == ce::Op::clzero && d.length == 5, "clzero with two REX bytes");
    check(dec({0xF2, 0x0F, 0x01, 0xFB}).op == ce::Op::none, "F2 0F 01 FB is not mwaitx");
    check(dec({0x66, 0x0F, 0x01, 0xFA}).op == ce::Op::none, "66 0F 01 FA is not monitorx");
    check(dec({0x66, 0x0F, 0x01, 0xFB}).op == ce::Op::none, "66 0F 01 FB is not mwaitx");
    {
        // An instruction is at most 15 bytes: 12 prefixes + 0F 01 FC fit, 13 do not.
        std::vector<uint8_t> v(12, 0x3E);
        v.insert(v.end(), {0x0F, 0x01, 0xFC});
        d = ce::decode(v.data(), v.size());
        check(d.op == ce::Op::clzero && d.length == 15, "clzero behind 12 prefixes (15 bytes)");
        v.insert(v.begin(), 0x3E);
        v.insert(v.end(), {0x90, 0x90});  // more readable bytes after it must not help
        check(ce::decode(v.data(), v.size()).op == ce::Op::none, "a 16-byte clzero is #UD");
        std::vector<uint8_t> w(11, 0x3E);
        w.insert(w.end(), {0x66, 0x0F, 0x78, 0xC0, 0x08, 0x04, 0x90});
        check(ce::decode(w.data(), w.size()).op == ce::Op::none, "a 17-byte extrq is #UD");
    }
    d = dec({0x48, 0x66, 0x0F, 0x78, 0xC1, 1, 1});
    check(d.op == ce::Op::extrq_imm && d.dst == 1, "a REX before 66 does not extend extrq's register");
    d = dec({0x66, 0x0F, 0x01, 0xFC});
    check(d.op == ce::Op::clzero && d.length == 4, "clzero with 66");
    d = dec({0x64, 0x48, 0x0F, 0x01, 0xFC});
    check(d.op == ce::Op::clzero && d.length == 5, "clzero with a segment override and REX");
    check(dec({0xF0, 0x0F, 0x01, 0xFC}).op == ce::Op::none, "LOCK clzero stays #UD");

    check(dec({0x66, 0x0F, 0xAE, 0x31}).length == 4, "clwb [rcx]");
    check(dec({0x66, 0x0F, 0xAE, 0x74, 0x01, 0x08}).length == 6, "clwb [rcx+rax+8]");
    check(dec({0x66, 0x0F, 0xAE, 0xB1, 0, 1, 0, 0}).length == 8, "clwb [rcx+disp32]");
    check(dec({0x66, 0x0F, 0xAE, 0x35, 1, 2, 3, 4}).length == 8, "clwb [rip+disp32]");
    check(dec({0x66, 0x0F, 0xAE, 0x34, 0x25, 1, 2, 3, 4}).length == 9, "clwb [disp32] through SIB");
    check(dec({0x66, 0x41, 0x0F, 0xAE, 0x30}).length == 5 && dec({0x66, 0x41, 0x0F, 0xAE, 0x30}).op == ce::Op::clwb,
          "clwb [r8]");
    check(dec({0x0F, 0xAE, 0x31}).op == ce::Op::none, "0F AE /6 without 66 is xsaveopt");
    check(dec({0x66, 0x0F, 0xAE, 0xF1}).op == ce::Op::none, "66 0F AE /6 with a register is not clwb");
    check(dec({0x66, 0x0F, 0xAE, 0x39}).op == ce::Op::none, "66 0F AE /7 is clflushopt");
    check(dec({0x66, 0x0F, 0xAE, 0x74, 0x01}).op == ce::Op::none, "a cut-off clwb is not decoded");
    check(dec({0x0F, 0x0B}).op == ce::Op::none, "ud2 is not handled");
}

void test_bits() {
    check(ce::insert_bits(0xFFFFFFFFFFFFFFFFull, 0, 8, 4) == 0xFFFFFFFFFFFFF00Full, "insert 8 at 4");
    check(ce::insert_bits(0, 0xABCD, 16, 48) == 0xABCD000000000000ull, "insert 16 at 48");
    check(ce::insert_bits(0x1234, 0xFEDCBA9876543210ull, 0, 0) == 0xFEDCBA9876543210ull, "insert len 0 is 64");
    check(ce::insert_bits(0, 0xFF, 8, 60) == 0xF000000000000000ull, "a field past bit 63 keeps what fits");
    check(ce::extract_bits(0xFEDCBA9876543210ull, 8, 4) == 0x21, "extract 8 at 4");
    check(ce::extract_bits(0xFEDCBA9876543210ull, 0, 0) == 0xFEDCBA9876543210ull, "extract len 0 is 64");
    check(ce::extract_bits(0xFEDCBA9876543210ull, 16, 56) == 0xFE, "extract past bit 63 zero-fills");
}

void test_apply() {
    CONTEXT c{};
    c.Rip = 0x1000;
    c.FltSave.XmmRegisters[0].Low = 0x1111111111111111ull;
    c.FltSave.XmmRegisters[0].High = 0x2222222222222222ll;
    c.FltSave.XmmRegisters[1].Low = 0xAB;
    check(ce::apply(dec({0xF2, 0x0F, 0x78, 0xC1, 0x08, 0x10}), &c), "apply insertq");
    check(c.FltSave.XmmRegisters[0].Low == 0x1111111111AB1111ull && c.Rip == 0x1006, "insertq result and rip");
    check(c.FltSave.XmmRegisters[0].High == 0, "insertq zeroes the upper quadword, as a Zen 3 does");

    c.FltSave.XmmRegisters[2].Low = 0xFFFF;
    c.FltSave.XmmRegisters[2].High = (20ll << 8) | 12;  // len 12 at bits 69:64, idx 20 at bits 77:72 (APM vol. 4)
    c.FltSave.XmmRegisters[3].Low = 0;
    check(ce::apply(dec({0xF2, 0x0F, 0x79, 0xDA}), &c), "apply insertq reg");  // insertq xmm3, xmm2
    check(c.FltSave.XmmRegisters[3].Low == (0xFFFull << 20), "insertq reg takes len and idx from the upper half");

    // Measured on a Zen 3 in #7111's review: control 0x0810 (len 16 in bits 5:0, idx 8 in bits 13:8 of the upper
    // quadword) inserts 16 bits of 0xAB at bit 8 of 0x1234.
    c.FltSave.XmmRegisters[6].Low = 0x1234;
    c.FltSave.XmmRegisters[7].Low = 0xAB;
    c.FltSave.XmmRegisters[7].High = 0x0810;
    check(ce::apply(dec({0xF2, 0x0F, 0x79, 0xF7}), &c) && c.FltSave.XmmRegisters[6].Low == 0xAB34,
          "insertq reg: the measured 0x0810 case");  // insertq xmm6, xmm7

    c.FltSave.XmmRegisters[4].Low = 0xFEDCBA9876543210ull;
    c.FltSave.XmmRegisters[5].Low = (4 << 8) | 8;  // idx 4 at bits 13:8, len 8 at bits 5:0
    check(ce::apply(dec({0x66, 0x0F, 0x79, 0xE5}), &c), "apply extrq reg");  // extrq xmm4, xmm5
    check(c.FltSave.XmmRegisters[4].Low == 0x21, "extrq reg result");

    alignas(64) static uint8_t line[128];
    std::memset(line, 0xEE, sizeof line);
    c.Rax = reinterpret_cast<uint64_t>(line + 64 + 17);
    const uint64_t rip = c.Rip;
    check(ce::apply(dec({0x0F, 0x01, 0xFC}), &c) && c.Rip == rip + 3, "apply clzero");
    bool zeroed = true, kept = true;
    for (int i = 64; i < 128; ++i) zeroed &= line[i] == 0;
    for (int i = 0; i < 64; ++i) kept &= line[i] == 0xEE;
    check(zeroed && kept, "clzero zeroes exactly the line rax points into");

    c.Rax = 0x10;  // the null page: not writable, so the write fault the CPU would raise is reported
    EXCEPTION_RECORD fault{};
    check(!ce::apply(dec({0x0F, 0x01, 0xFC}), &c, &fault) && c.Rip == rip + 3,
          "clzero on an unwritable line is refused and leaves rip");
    check(fault.ExceptionCode == STATUS_ACCESS_VIOLATION && fault.NumberParameters == 2 &&
              fault.ExceptionInformation[0] == 1 && fault.ExceptionInformation[1] == 0,
          "clzero on an unwritable line reports a write access violation at the line");
}

// ---- Running the real encodings ----

using Fn = void (*)(const void* in, void* out);

struct Code {
    uint8_t* mem = nullptr;
    explicit Code(std::initializer_list<uint8_t> bytes) {
        mem = static_cast<uint8_t*>(VirtualAlloc(nullptr, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
        if (mem) std::memcpy(mem, bytes.begin(), bytes.size());
        FlushInstructionCache(GetCurrentProcess(), mem, 4096);
    }
    ~Code() { if (mem) VirtualFree(mem, 0, MEM_RELEASE); }
    Fn fn() const { return reinterpret_cast<Fn>(mem); }
};

bool run(const Code& code, const void* in, void* out, DWORD* exception) {
    *exception = 0;
    __try {
        code.fn()(in, out);
        return true;
    } __except (*exception = GetExceptionCode(), EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

const char* path(bool host_has) { return host_has ? "native" : "emulated"; }

void test_live(const ce::Host& host) {
    DWORD ex = 0;
    alignas(32) uint64_t in[8] = {}, out[8] = {};

    // movdqu xmm0,[rcx]; movdqu xmm1,[rcx+16]; insertq xmm0,xmm1,12,20; movdqu [rdx],xmm0; ret
    Code insq({0xF3, 0x0F, 0x6F, 0x01, 0xF3, 0x0F, 0x6F, 0x49, 0x10, 0xF2, 0x0F, 0x78, 0xC1, 12, 20,
               0xF3, 0x0F, 0x7F, 0x02, 0xC3});
    in[0] = 0x0123456789ABCDEFull;
    in[1] = 0x5555555555555555ull;
    in[2] = 0xFFFFFFFFFFFFFABCull;
    check(run(insq, in, out, &ex), "insertq imm ran");
    check(out[0] == ce::insert_bits(in[0], in[2], 12, 20), "insertq imm result");
    std::printf("insertq imm: %s\n", path(host.sse4a));

    // same with the register form: insertq xmm0, xmm1 (len in xmm1[69:64], idx in xmm1[77:72])
    Code insr({0xF3, 0x0F, 0x6F, 0x01, 0xF3, 0x0F, 0x6F, 0x49, 0x10, 0xF2, 0x0F, 0x79, 0xC1,
               0xF3, 0x0F, 0x7F, 0x02, 0xC3});
    in[3] = (7ull << 8) | 33;  // len 33, idx 7
    check(run(insr, in, out, &ex), "insertq reg ran");
    check(out[0] == ce::insert_bits(in[0], in[2], 33, 7), "insertq reg result");

    // movdqu xmm0,[rcx]; extrq xmm0,9,13; movdqu [rdx],xmm0; ret
    Code exti({0xF3, 0x0F, 0x6F, 0x01, 0x66, 0x0F, 0x78, 0xC0, 9, 13, 0xF3, 0x0F, 0x7F, 0x02, 0xC3});
    check(run(exti, in, out, &ex), "extrq imm ran");
    check(out[0] == ce::extract_bits(in[0], 9, 13), "extrq imm result");

    // movdqu xmm0,[rcx]; movdqu xmm1,[rcx+16]; extrq xmm0,xmm1; movdqu [rdx],xmm0; ret
    Code extr({0xF3, 0x0F, 0x6F, 0x01, 0xF3, 0x0F, 0x6F, 0x49, 0x10, 0x66, 0x0F, 0x79, 0xC1,
               0xF3, 0x0F, 0x7F, 0x02, 0xC3});
    in[2] = (40ull << 8) | 20;
    check(run(extr, in, out, &ex), "extrq reg ran");
    check(out[0] == ce::extract_bits(in[0], 20, 40), "extrq reg result");
    std::printf("extrq: %s\n", path(host.sse4a));

    // The upper half of ymm0 survives an emulated (legacy SSE) insertq:
    // vmovdqu ymm0,[rcx]; vmovdqu ymm1,[rcx+32]; insertq xmm0,xmm1,8,8; vmovdqu [rdx],ymm0; vzeroupper; ret
    Code ymm({0xC5, 0xFE, 0x6F, 0x01, 0xC5, 0xFE, 0x6F, 0x49, 0x20, 0xF2, 0x0F, 0x78, 0xC1, 8, 8,
              0xC5, 0xFE, 0x7F, 0x02, 0xC5, 0xF8, 0x77, 0xC3});
    for (int i = 0; i < 8; ++i) in[i] = 0x1010101010101010ull * (i + 1);
    std::memset(out, 0, sizeof out);
    check(run(ymm, in, out, &ex), "insertq between AVX code ran");
    check(out[0] == ce::insert_bits(in[0], in[4], 8, 8), "insertq between AVX code: low quadword");
    check(out[2] == in[2] && out[3] == in[3], "insertq keeps ymm0[255:128]");
    if (!host.sse4a) check(out[1] == 0, "emulated insertq zeroes xmm0[127:64], as a Zen 3 does");

    // mov rax,rcx; clzero; ret
    Code clz({0x48, 0x89, 0xC8, 0x0F, 0x01, 0xFC, 0xC3});
    alignas(64) static uint8_t lines[192];
    std::memset(lines, 0x5A, sizeof lines);
    check(run(clz, lines + 64 + 40, nullptr, &ex), "clzero ran");
    bool ok = true;
    for (int i = 0; i < 192; ++i) ok &= lines[i] == ((i >= 64 && i < 128) ? 0 : 0x5A);
    check(ok, "clzero zeroed one line");
    std::printf("clzero: %s\n", path(host.clzero));

    // A read-only line: the CPU and the handler both raise a write access violation, not #UD.
    if (auto* ro = static_cast<uint8_t*>(VirtualAlloc(nullptr, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE))) {
        DWORD old = 0;
        VirtualProtect(ro, 4096, PAGE_READONLY, &old);
        check(!run(clz, ro + 128, nullptr, &ex) && ex == STATUS_ACCESS_VIOLATION,
              "clzero on a read-only line raises an access violation");
        VirtualFree(ro, 0, MEM_RELEASE);
    }
    // The same after the page was written, so it is resident and the handler answers from the working set: it must
    // still see the read-only protection, and a guard page must still report its guard.
    if (auto* ro = static_cast<uint8_t*>(VirtualAlloc(nullptr, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE))) {
        std::memset(ro, 0x5A, 4096);
        DWORD old = 0;
        VirtualProtect(ro, 4096, PAGE_READONLY, &old);
        check(!run(clz, ro + 128, nullptr, &ex) && ex == STATUS_ACCESS_VIOLATION,
              "clzero on a resident read-only line raises an access violation");
        VirtualProtect(ro, 4096, PAGE_READWRITE | PAGE_GUARD, &old);
        check(!run(clz, ro + 128, nullptr, &ex) && ex == STATUS_GUARD_PAGE_VIOLATION,
              "clzero on a resident guard page raises a guard-page violation");
        check(run(clz, ro + 128, nullptr, &ex) && ro[128] == 0 && ro[191] == 0 && ro[192] == 0x5A,
              "clzero after the guard is gone zeroes the line");
        VirtualFree(ro, 0, MEM_RELEASE);
    }
    if (!host.clzero) {  // the emulated path accepts the 66 prefix: mov rax,rcx; 66 clzero; ret
        Code clz66({0x48, 0x89, 0xC8, 0x66, 0x0F, 0x01, 0xFC, 0xC3});
        std::memset(lines, 0x5A, sizeof lines);
        check(run(clz66, lines + 130, nullptr, &ex) && lines[128] == 0 && lines[191] == 0 && lines[127] == 0x5A,
              "clzero with 66 emulated");
    }

    // mov rax,rcx; xor ecx,ecx; xor edx,edx; monitorx; xor eax,eax; xor ecx,ecx; mwaitx; ret
    // (no timer: an interrupt or a store ends a native wait; the emulated one returns at once)
    Code mon({0x48, 0x89, 0xC8, 0x31, 0xC9, 0x31, 0xD2, 0x0F, 0x01, 0xFA, 0x31, 0xC0, 0x31, 0xC9, 0x0F, 0x01, 0xFB,
              0xC3});
    check(run(mon, lines, nullptr, &ex), "monitorx/mwaitx ran");
    std::printf("monitorx/mwaitx: %s\n", path(host.monitorx));

    // xor eax,eax; clwb [rcx]; clwb [rcx+rax+8]; mov dword [rdx],1; ret
    Code wb({0x31, 0xC0, 0x66, 0x0F, 0xAE, 0x31, 0x66, 0x0F, 0xAE, 0x74, 0x01, 0x08, 0xC7, 0x02, 0x01, 0, 0, 0,
             0xC3});
    uint32_t marker = 0;
    check(run(wb, lines, &marker, &ex) && marker == 1, "clwb ran and execution continued after it");
    std::printf("clwb: %s\n", path(host.clwb));

    // A #UD the handler does not know stays an illegal-instruction exception: ud2; ret
    Code ud({0x0F, 0x0B, 0xC3});
    check(!run(ud, nullptr, nullptr, &ex) && ex == STATUS_ILLEGAL_INSTRUCTION, "ud2 still faults");
}
}  // namespace

int main() {
    test_decode();
    test_bits();
    test_apply();
    const ce::Host host = ce::detect_host();
    std::printf("host: SSE4a %d, MONITORX %d, CLZERO %d, CLWB %d\n", host.sse4a, host.monitorx, host.clzero,
                host.clwb);
    check(ce::install(), "install the handler");
    test_live(host);
    if (failures) {
        std::fprintf(stderr, "%d failure(s)\n", failures);
        return 1;
    }
    std::printf("cpu_emulation: all checks passed\n");
    return 0;
}
