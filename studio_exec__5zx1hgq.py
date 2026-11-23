import os, subprocess
os.chdir(r"F:\Work\Leonida")
def resolve(f, decisions):
    t = open(f, encoding="utf-8").read()
    pos = 0; ci = 0
    while True:
        s = t.find("<<<<<<<", pos)
        if s < 0: break
        e = t.find(">>>>>>> pr/7052", s); m = t.find("=======", s)
        head = t[s+len("<<<<<<< HEAD\n"):m].rstrip("\n")
        pr = t[m+7:e].rstrip("\n")
        d = decisions[ci]
        merged = (head + "\n" + pr) if d=="both" else (head if d=="head" else pr) if not callable(d) else d(head,pr)
        t = t[:s] + merged + t[e+len(">>>>>>> pr/7052"):]
        pos = s + len(merged); ci += 1
    open(f,"w",encoding="utf-8",newline="\n").write(t)
    assert "<<<<<<<" not in t, f

def c2(h,p):
    return """    if (slot <= 2 || slot == 8 || slot == 9 || slot == 14 || slot == 26 || slot == 31 || slot == 32 || slot == 34 ||
        slot == 35 || slot == 36 || slot == 45 || slot == 46 || slot == 50 || slot == 62 || slot == 67 ||
        slot == 70 || slot == 75 || slot == 76 || slot == 133 || slot == kCreateCommittedResource3X)"""
resolve("runtime/shims/d3d12_xs_device.cpp", ["both", c2])

def h2(h,p):
    return """// proxies), 9 (allocator proxies), 14, 26 (the console's struct-return form), 31/34/35/45/46/50 (proxies unwrapped),
// 32 and 36 (fence proxies), 48/73 and 78/79/80 (the Xbox slots above, at their configured indices), 62, 67, 70, 76,
// 127 and 133. Slot 10 is intentionally false: the measured 9/10 handle conversion is on the heap proxy, and device
// slot 9 is CreateCommandAllocator, which the proxy handles for the allocator proxy."""
resolve("runtime/shims/d3d12_xs_device.h", ["both", h2])

def m1(h,p):
    return """        check_alloc_failures() || check_allocators() || check_xbox_slots() || check_custom_heap_properties() ||
        check_slot70() || check_slot70_raw_buffer() || check_committed_resource3_x())"""
resolve("runtime/test/xs_proxy_mock_test.cpp", [m1])
subprocess.run(["git","add","-A"], capture_output=True)
r = subprocess.run(["git","commit","-q","-m","stack: merge PR #7052 (slot 127 committed resource 3) onto new main"], capture_output=True, text=True)
print("committed" if r.returncode==0 else r.stderr[:300])
r = subprocess.run(["git","status","--porcelain"], capture_output=True, text=True)
print("clean:", not r.stdout.strip())
