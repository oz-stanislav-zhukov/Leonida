import struct
from capstone import Cs, CS_ARCH_X86, CS_MODE_64

path = r"F:\GTA6\GTA6.Xbox.v1.0.2.1-CYBERLEEK\game_scarlett_master.exe"
data = open(path, "rb").read()
print("file size:", len(data))

pe_off = struct.unpack_from("<I", data, 0x3C)[0]
print("first 64 bytes:", data[:64].hex())
print("pe_off:", hex(pe_off))
sig = data[pe_off:pe_off+4]
print("sig:", sig)
machine, nsec = struct.unpack_from("<HH", data, pe_off + 4)
opt_off = pe_off + 4 + 20
magic = struct.unpack_from("<H", data, opt_off)[0]
print(f"machine={machine:#x} nsec={nsec} magic={magic:#x}")
assert sig == b"PE\0\0", sig
coff = pe_off + 4
is64 = magic == 0x20b
n_rva = struct.unpack_from("<I", data, opt_off + 108)[0]
sect_off = opt_off + 112 + n_rva * 8
print(f"n_rva={n_rva} sect_off={sect_off:#x}")
secs = []
for i in range(nsec):
    o = sect_off + i*40
    name = data[o:o+8].rstrip(b"\0").decode(errors="replace")
    vsize, vaddr, rawsize, rawptr = struct.unpack_from("<IIII", data, o+8)
    secs.append((name, vaddr, vsize, rawptr, rawsize))

print("sections:")
for s in secs:
    print(f"  {s[0]:>10} vaddr={s[1]:#x} vsize={s[2]:#x} raw={s[3]:#x}/{s[4]:#x}")

def rva2off(rva):
    for name, vaddr, vsize, rawptr, rawsize in secs:
        if vaddr <= rva < vaddr + max(vsize, rawsize):
            return rawptr + (rva - vaddr)
    return None

crash = 0x51d1adc
off = rva2off(crash)
print(f"crash RVA {crash:#x} -> file offset {off:#x}")

md = Cs(CS_ARCH_X86, CS_MODE_64)

# find the enclosing function via .pdata (RUNTIME_FUNCTION table)
pdata = [s for s in secs if s[0] == ".pdata"][0]
pdata_raw, pdata_vsize = pdata[3], pdata[2]
rfs = []
for i in range(pdata_vsize // 12):
    o = pdata_raw + i * 12
    start_rva, end_rva, unw_off = struct.unpack_from("<III", data, o)
    rfs.append((start_rva, end_rva))
enc = [r for r in rfs if r[0] <= crash < r[1]]
print(f"runtime functions: {len(rfs)}, enclosing crash: {[f'{a:#x}..{b:#x}' for a,b in enc]}")
if enc:
    fstart = enc[0][0]
    fend = enc[0][1]
else:
    # nearest function before crash
    cands = [r for r in rfs if r[1] <= crash]
    fstart, fend = (cands[-1] if cands else (crash - 0x200, crash + 0x200))
    print("using nearest-before function")
foff = rva2off(fstart)
fend_off = rva2off(min(fend, fstart + 0x400))
chunk = data[foff:fend_off]
lines = []
for ins in md.disasm(chunk, fstart):
    mark = " <<<CRASH" if ins.address == crash else (" <<<=next" if ins.address > crash and not lines[-1].endswith("<<<=next") and any(l.endswith('<<<CRASH') for l in lines) else "")
    lines.append(f"{ins.address:#x}: {ins.mnemonic} {ins.op_str}{mark}")
    if ins.address > crash + 0x20:
        break
print(f"function {fstart:#x}..{fend:#x}, showing up to crash+0x20 ({len(lines)} insns)")
print("\n".join(lines[-60:]))

# --- .tls template contents around +0x12a8 and relocations over .tls ---
tls = [s for s in secs if s[0] == ".tls"][0]
print(f"\n.tls vaddr={tls[1]:#x} vsize={tls[2]:#x} raw={tls[3]:#x}")
off12a8 = tls[3] + 0x12a8
print("bytes at .tls+0x12a8:", data[off12a8:off12a8+16].hex())
print("qword at .tls+0x12a8 =", hex(struct.unpack_from('<q', data, off12a8)[0]))
for d in (0x126e0, 0x126e4, 0x126e8):
    o = tls[3] + d
    print(f"dword at .tls+{d:#x} =", hex(struct.unpack_from('<I', data, o)[0]))

reloc = [s for s in secs if s[0] == ".reloc"][0]
rlo, rlv = reloc[3], reloc[2]
n = 0
hits = []
off = rlo
while off < rlo + rlv:
    page_rva, size = struct.unpack_from('<II', data, off)
    entries = (size - 8) // 4
    for i in range(entries):
        e = struct.unpack_from('<I', data, off + 8 + i*4)[0]
        rtype, roff = e >> 12, e & 0xfff
        rva = page_rva + roff
        if tls[1] <= rva < tls[1] + tls[2]:
            hits.append((rva, rtype))
        n += 1
    off += size
print(f"total reloc entries: {n}, over .tls: {len(hits)}")
for rva, rt in hits[:20]:
    print(f"  reloc {rva:#x} (tls+{rva - tls[1]:#x}) type={rt}")

# --- constants referenced by the crashing function (RIP-relative) ---
def rdq(rva):
    o = rva2off(rva)
    if o is None or o + 8 > len(data):
        return f"out-of-file (rva {rva:#x} -> off {o})"
    return data[o:o+8].hex()
print(f"\nconst @ +0x51d1aed target 0x{0x51d1af2 + 0x5771ddd:x}: {rdq(0x51d1af2 + 0x5771ddd)}")
print(f"const @ +0x51d1afd target 0x{0x51d1b00 + 0xF880BB:x}: {rdq(0x51d1b00 + 0xF880BB)}")

# --- the caller of the small getter: enclose +0x51d1aa5, show its head ---
enc2 = [r for r in rfs if r[0] <= 0x51d1aa5 < r[1]]
if enc2:
    cs, ce = enc2[0]
    print(f"\ncaller function {cs:#x}..{ce:#x} (size {ce-cs:#x})")
    coff = rva2off(cs)
    clines = []
    for ins in md.disasm(data[coff:coff+min(ce-cs, 0x300)], cs):
        clines.append(f"{ins.address:#x}: {ins.mnemonic} {ins.op_str}")
        if ins.address > 0x51d1ab0:
            break
    print("\n".join(clines[-45:]))
