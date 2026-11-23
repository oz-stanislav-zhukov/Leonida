import os, subprocess
os.chdir(r"F:\Work\Leonida")
f = "runtime/test/xs_proxy_mock_test.cpp"
t = open(f, encoding="utf-8").read()
pos = 0; ci = 0
decisions = ["both", "both", "pr", "both"]
while True:
    s = t.find("<<<<<<<", pos)
    if s < 0 or ci >= len(decisions): break
    e = t.find(">>>>>>> pr/7052", s); m = t.find("=======", s)
    head = t[s+len("<<<<<<< HEAD\n"):m].rstrip("\n")
    pr = t[m+7:e].rstrip("\n")
    d = decisions[ci]
    merged = (head + "\n" + pr) if d=="both" else (head if d=="head" else pr)
    t = t[:s] + merged + t[e+len(">>>>>>> pr/7052"):]
    pos = s + len(merged); ci += 1
s = t.find("<<<<<<<"); e = t.find(">>>>>>> pr/7052", s); m = t.find("=======", s)
newm = """    if (check_tail(true) || check_levels() || check_slot48() || check_fence1() || check_create_failures() ||
        check_open_shared() || check_alloc_failures() || check_allocators() || check_xbox_slots() ||
        check_descriptor_heap_hooks() || check_slot70() || check_slot70_raw_buffer() ||
        check_committed_resource3_x())"""
t = t[:s] + newm + t[e+len(">>>>>>> pr/7052"):]
open(f,"w",encoding="utf-8",newline="\n").write(t)
assert "<<<<<<<" not in t
print("conflicts resolved")

t = open(f, encoding="utf-8").read()
old = """    g_committed3_133 = {};
    EXPECT(hr_of(raw6(device10, 133, kVa, word(&target), 6, word(&clear), word(&kResourceIid), word(&resource))) ==
           S_OK);
    EXPECT(resource && g_committed3_133.calls == 1 && mock10->calls[27] == 0);
    EXPECT(g_committed3_133.layout == 6 && g_committed3_133.clear == &clear && !g_committed3_133.session &&
           g_committed3_133.castable_count == 0 && g_committed3_133.riid == &kResourceIid);
    EXPECT(g_committed3_133.heap.Type == D3D12_HEAP_TYPE_DEFAULT);
    EXPECT(g_committed3_133.desc.Flags == D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET &&
           g_committed3_133.desc.Layout == D3D12_TEXTURE_LAYOUT_UNKNOWN && g_committed3_133.desc.Width == 64);
    EXPECT(release(resource) == 0);
    // A buffer goes in UNDEFINED whatever word it came with.
    EXPECT(hr_of(raw6(device10, 133, kVa, word(&buffer), 2, 0, word(&kResourceIid), word(&resource))) == S_OK);
    EXPECT(g_committed3_133.calls == 2 && g_committed3_133.layout == 0xFFFFFFFFu && !g_committed3_133.clear);
    EXPECT(release(resource) == 0);
    EXPECT(release(device10) == 0 && mock10->refs == 0);"""
new = """    g_committed3 = {};
    EXPECT(hr_of(raw6(device10, 133, kVa, word(&target), 6, word(&clear), word(&kResourceIid), word(&resource))) ==
           S_OK);
    EXPECT(resource && g_committed3.calls == 1 && mock10->calls[27] == 0);
    EXPECT(g_committed3.layout == 6 && g_committed3.clear == &clear && !g_committed3.session &&
           g_committed3.castable_count == 0 && g_committed3.riid == &kResourceIid);
    EXPECT(g_committed3.heap.Type == D3D12_HEAP_TYPE_DEFAULT);
    EXPECT(g_committed3.desc.Flags == D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET &&
           g_committed3.desc.Layout == D3D12_TEXTURE_LAYOUT_UNKNOWN && g_committed3.desc.Width == 64);
    EXPECT(release(resource) == 0);
    // A buffer goes in UNDEFINED whatever word it came with (plain flags: the shared PC slot 76 mock refuses a
    // buffer with render-target/UAV flags).
    D3D12_RESOURCE_DESC plain_buffer = buffer;
    plain_buffer.Flags = D3D12_RESOURCE_FLAG_NONE;
    EXPECT(hr_of(raw6(device10, 133, kVa, word(&plain_buffer), 2, 0, word(&kResourceIid), word(&resource))) == S_OK);
    EXPECT(g_committed3.calls == 2 && g_committed3.layout == 0xFFFFFFFFu && !g_committed3.clear);
    EXPECT(release(resource) == 0);
    EXPECT(release(device10) == 0 && mock10->refs == 0);
    g_committed3 = {};  // leave the shared mock clean for check_committed_resource3_x"""
assert t.count(old)==1
t = t.replace(old, new)
open(f,"w",encoding="utf-8",newline="\n").write(t)
subprocess.run(["git","add","-A"], capture_output=True)
r = subprocess.run(["git","commit","-q","-m","stack: merge PR #7052 (hammokko boot stack)"], capture_output=True, text=True)
print("committed" if r.returncode==0 else r.stderr[:300])
