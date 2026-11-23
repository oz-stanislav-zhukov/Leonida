import os, subprocess
os.chdir(r"F:\Work\Leonida")

# 1. cpp: union of slot lists + keep routes fallback
f = "runtime/shims/d3d12_xs_device.cpp"
t = open(f, encoding="utf-8").read()
old = """<<<<<<< HEAD
    if (slot <= 2 || slot == 8 || slot == 9 || slot == 14 || slot == 31 || slot == 32 || slot == 34 || slot == 35 ||
        slot == 36 || slot == 45 || slot == 46 || slot == 50 || slot == 62 || slot == 67 ||
        slot == 75 || slot == 76 || slot == 133)
        return true;
    const XboxDeviceSlotRoutes& routes = xbox_device_slot_routes();
    return slot == routes.frame_interval || slot == routes.schedule_event || slot == routes.wait_event ||
           slot == routes.queue_x || slot == routes.list_x;
=======
    return slot <= 2 || slot == 8 || slot == 9 || slot == 14 || (slot >= 17 && slot <= 24) || slot == 31 ||
           slot == 32 || slot == 34 || slot == 35 || slot == 36 || slot == 45 || slot == 46 || slot == 50 ||
           slot == 62 || slot == 67 || slot == 75 || slot == 76;
>>>>>>> pr"""
new = """    if (slot <= 2 || slot == 8 || slot == 9 || slot == 14 || (slot >= 17 && slot <= 24) || slot == 26 ||
        slot == 31 || slot == 32 || slot == 34 || slot == 35 || slot == 36 || slot == 45 || slot == 46 ||
        slot == 50 || slot == 62 || slot == 67 || slot == 75 || slot == 76 || slot == 133)
        return true;
    const XboxDeviceSlotRoutes& routes = xbox_device_slot_routes();
    return slot == routes.frame_interval || slot == routes.schedule_event || slot == routes.wait_event ||
           slot == routes.queue_x || slot == routes.list_x;"""
assert t.count(old)==1
open(f,"w",encoding="utf-8",newline="\n").write(t.replace(old,new))

# 2. header doc: combined
f = "runtime/shims/d3d12_xs_device.h"
t = open(f, encoding="utf-8").read()
old = """<<<<<<< HEAD
// proxies), 9 (allocator proxies), 14, 31/34/35/45/46/50 (proxies unwrapped), 32 and 36 (fence proxies), 48/73 and
// 78/79/80 (the Xbox slots above, at their configured indices), 62, 67 and 76. Slot 10 is intentionally false: the
// measured 9/10 handle conversion is on the heap proxy, and device slot 9 is CreateCommandAllocator, which the proxy
// handles for the allocator proxy.
=======
// proxies), 9 (allocator proxies), 14 (descriptor heap proxy), 17-24 (CPU descriptor handle translation),
// 31/34/35/45/46/50 (proxies unwrapped), 32 and 36 (fence proxies), 62, 67 and 76. Slot 10 is intentionally false:
// the measured 9/10 handle conversion is on the heap proxy, and device slot 9 is CreateCommandAllocator, which the
// proxy handles for the allocator proxy.
>>>>>>> pr"""
new = """// proxies), 9 (allocator proxies), 14 (descriptor heap proxy), 17-24 (CPU descriptor handle translation),
// 31/34/35/45/46/50 (proxies unwrapped), 32 and 36 (fence proxies), 48/73 and 78/79/80 (the Xbox slots above, at
// their configured indices), 62, 67 and 76. Slot 10 is intentionally false: the measured 9/10 handle conversion is
// on the heap proxy, and device slot 9 is CreateCommandAllocator, which the proxy handles for the allocator proxy."""
assert t.count(old)==1
open(f,"w",encoding="utf-8",newline="\n").write(t.replace(old,new))

# 3+4. mock test
f = "runtime/test/xs_proxy_mock_test.cpp"
t = open(f, encoding="utf-8").read()
old = """<<<<<<< HEAD
enum class Kind { device, queue, fence, resource, allocator, command_list };
=======
enum class Kind { device, queue, fence, resource, allocator, heap };
>>>>>>> pr"""
new = "enum class Kind { device, queue, fence, resource, allocator, command_list, heap };"
assert t.count(old)==1
t = t.replace(old,new)
old = """<<<<<<< HEAD
    if (check_tail(true) || check_levels() || check_slot48() || check_fence1() || check_create_failures() ||
        check_open_shared() || check_alloc_failures() || check_allocators() || check_xbox_slots())
=======
    if (check_tail(true) || check_levels() || check_fence1() || check_create_failures() || check_open_shared() ||
        check_alloc_failures() || check_allocators() || check_descriptor_heap_hooks())
>>>>>>> pr"""
new = """    if (check_tail(true) || check_levels() || check_slot48() || check_fence1() || check_create_failures() ||
        check_open_shared() || check_alloc_failures() || check_allocators() || check_xbox_slots() ||
        check_descriptor_heap_hooks())"""
assert t.count(old)==1
t = t.replace(old,new)
open(f,"w",encoding="utf-8",newline="\n").write(t)

for f in ["runtime/shims/d3d12_xs_device.cpp","runtime/shims/d3d12_xs_device.h","runtime/test/xs_proxy_mock_test.cpp"]:
    assert "<<<<<<<" not in open(f,encoding="utf-8").read(), f
subprocess.run(["git","add","-A"], capture_output=True)
r = subprocess.run(["git","commit","-q","-m","stack: merge PR #7021 (hammokko boot stack)"], capture_output=True, text=True)
print("committed" if r.returncode==0 else r.stderr[:300])
