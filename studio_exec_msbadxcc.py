import os, subprocess
os.chdir(r"F:\Work\Leonida")
f = "runtime/CMakeLists.txt"
t = open(f, encoding="utf-8").read()
i = t.find("<<<<<<<"); e = t.find(">>>>>>>", i); m = t.find("=======", i)
head = t[i+len("<<<<<<< HEAD\n"):m].rstrip("\n")
pr = t[m+7:e].rstrip("\n")
t = t[:i] + head + "\n" + pr + t[e+len(">>>>>>> f60409a3b (shims: console DirectStorage (DStorageGetFactory) with file-to-memory reads (#8461 code only)"):]
open(f,"w",encoding="utf-8",newline="\n").write(t)
assert "<<<<<<<" not in t
subprocess.run(["git","add","-A"], capture_output=True)
r = subprocess.run(["git","commit","-q","-m","shims: console DirectStorage (DStorageGetFactory) with file-to-memory reads (port of #8461 onto new main)"], capture_output=True, text=True)
print("committed" if r.returncode==0 else r.stderr[:300])
# now xapu/chad
r = subprocess.run(["git","cherry-pick","-n","8b123ca7a"], capture_output=True, text=True)
out = r.stdout+r.stderr
conf = [l for l in out.splitlines() if "CONFLICT" in l or "error" in l]
print("xapu cp:", conf[:5] or "clean")
r2 = subprocess.run(["git","status","--porcelain"], capture_output=True, text=True)
print([l[:70] for l in r2.stdout.splitlines() if l.strip()][:12])
