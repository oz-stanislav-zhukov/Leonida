import os, subprocess
os.chdir(r"F:\Work\Leonida")
def resolve(f, decisions):
    t = open(f, encoding="utf-8").read()
    pos = 0; ci = 0
    while True:
        s = t.find("<<<<<<<", pos)
        if s < 0: break
        e = t.find(">>>>>>> pr/6964", s); m = t.find("=======", s)
        head = t[s+len("<<<<<<< HEAD\n"):m].rstrip("\n")
        pr = t[m+7:e].rstrip("\n")
        d = decisions[ci]
        merged = (head + "\n" + pr) if d=="both" else (head if d=="head" else pr) if not callable(d) else d(head,pr)
        t = t[:s] + merged + t[e+len(">>>>>>> pr/6964"):]
        pos = s + len(merged); ci += 1
    open(f,"w",encoding="utf-8",newline="\n").write(t)
    assert "<<<<<<<" not in t, f

def c1(h,p):
    return """// are IUnknown (0-2), slot 14 (which wraps the heap), 26 (the title passes the return buffer in RCX and the device in
// RDX), 48 (a queue creation, where the PC has OpenExistingHeapFromAddress), 62 and 76, plus the heap proxy's 9/10
// conversion from the PC hidden-pointer form to the Xbox RAX form. Slots 8/75 (command queues), 36 (fences) and 32
// (a shared fence) wrap what they create in queue and fence proxies built the same way, and slots 31/34/35/45/46/50
// hand the real objects behind heap, queue and fence proxies to the real device (see the command queue and fence
// section)."""
def c3(h,p):
    return """    if (slot <= 2 || slot == 8 || slot == 9 || slot == 14 || slot == 26 || slot == 31 || slot == 32 || slot == 34 ||
        slot == 35 || slot == 36 || slot == 45 || slot == 46 || slot == 50 || slot == 62 || slot == 67 ||
        slot == 75 || slot == 76 || slot == 133)"""
resolve("runtime/shims/d3d12_xs_device.cpp", [c1, "both", c3])
resolve("runtime/test/xs_proxy_mock_test.cpp", ["both"])
subprocess.run(["git","add","-A"], capture_output=True)
r = subprocess.run(["git","commit","-q","-m","stack: merge PR #6964 (slot 26 console form) onto new main"], capture_output=True, text=True)
print("committed" if r.returncode==0 else r.stderr[:200])
