import os, subprocess
os.chdir(r"F:\Work\Leonida")
f = "runtime/test/xs_proxy_mock_test.cpp"
t = open(f, encoding="utf-8").read()
pos = 0; ci = 0
while True:
    s = t.find("<<<<<<<", pos)
    if s < 0: break
    e = t.find(">>>>>>> pr/7066", s); m = t.find("=======", s)
    head = t[s+len("<<<<<<< HEAD\n"):m].rstrip("\n")
    pr = t[m+7:e].rstrip("\n")
    merged = head + "\n" + pr.strip()
    t = t[:s] + merged + t[e+len(">>>>>>> pr/7066"):]
    pos = s + len(merged); ci += 1
open(f,"w",encoding="utf-8",newline="\n").write(t)
assert "<<<<<<<" not in t
print("mock ok | check_slot70 def:", t.count("int check_slot70()"), "| custom heap def:", t.count("int check_custom_heap_properties()"))
subprocess.run(["git","add","-A"], capture_output=True)
r = subprocess.run(["git","commit","-q","-m","stack: merge PR #7066 (slot 70 raw SRV) onto new main"], capture_output=True, text=True)
print("committed" if r.returncode==0 else r.stderr[:300])
# verify no conflict markers anywhere in tree
r = subprocess.run(["git","grep","-l","^<<<<<<< ","HEAD"], capture_output=True, text=True)
print("marker files:", r.stdout or "none")
