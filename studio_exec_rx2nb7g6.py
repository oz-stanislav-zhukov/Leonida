import os, subprocess
os.chdir(r"F:\Work\Leonida")
def resolve(f, decisions):
    t = open(f, encoding="utf-8").read()
    pos = 0; ci = 0
    while True:
        s = t.find("<<<<<<<", pos)
        if s < 0: break
        e = t.find(">>>>>>> pr/7110", s); m = t.find("=======", s)
        head = t[s+len("<<<<<<< HEAD\n"):m].rstrip("\n")
        pr = t[m+7:e].rstrip("\n")
        d = decisions[ci]
        merged = (head + "\n" + pr) if d=="both" else (head if d=="head" else pr) if not callable(d) else d(head,pr)
        t = t[:s] + merged + t[e+len(">>>>>>> pr/7110"):]
        pos = s + len(merged); ci += 1
    open(f,"w",encoding="utf-8",newline="\n").write(t)
    assert "<<<<<<<" not in t, f

# docs: take PR (it supersedes our sentence about slot 25)
resolve("docs/d3d12_vulkan.md", ["pr"])

def c1(h,p):
    return """// arguments only, so the x64 registers and stack words line up exactly; methods that return a struct keep their
// hidden return pointer in RDX, which is passed through as the first argument unchanged. The measured exceptions
// are IUnknown (0-2), slot 14 (which wraps the heap), 25 and 26 (the title passes the return buffer in RCX and the
// device in RDX), 48 (a queue creation, where the PC has OpenExistingHeapFromAddress), 62 and 76, plus the heap
// proxy's 9/10 conversion from the PC hidden-pointer form to the Xbox RAX form. Slots 8/75 (command queues), 36
// (fences) and 32 (a shared fence) wrap what they create in queue and fence proxies built the same way, and slots
// 31/34/35/45/46/50 hand the real objects behind heap, queue and fence proxies to the real device (see the command
// queue and fence section)."""
def c2(h,p):
    return """    if (slot <= 2 || slot == 8 || slot == 9 || slot == 14 || slot == 25 || slot == 26 || slot == 31 || slot == 32 ||
        slot == 34 || slot == 35 || slot == 36 || slot == 45 || slot == 46 || slot == 50 || slot == 56 || slot == 62 ||
        slot == 67 || slot == 70 || slot == 75 || slot == 76 || slot == 133 || slot == kCreateCommittedResource3X)"""
resolve("runtime/shims/d3d12_xs_device.cpp", [c1, c2])

def h1(h,p):
    return p + "\n" + h  # PR doc block first (slot 25/26), then our slot 48 block
resolve("runtime/shims/d3d12_xs_device.h", [h1])

def m1(h,p):
    return """        check_alloc_failures() || check_allocators() || check_render_target_sizes() ||
        check_allocation_descriptions_cleaned() || check_render_target_layout() || check_xbox_slots() ||
        check_custom_heap_properties() || check_slot70() || check_slot70_raw_buffer() ||
        check_committed_resource3_x())"""
resolve("runtime/test/xs_proxy_mock_test.cpp", [m1])
subprocess.run(["git","add","-A"], capture_output=True)
r = subprocess.run(["git","commit","-q","-m","stack: merge PR #7110 (slots 25/56 allocation info) onto new main"], capture_output=True, text=True)
print("committed" if r.returncode==0 else r.stderr[:300])
r = subprocess.run(["git","status","--porcelain"], capture_output=True, text=True)
print("clean:", not r.stdout.strip())
