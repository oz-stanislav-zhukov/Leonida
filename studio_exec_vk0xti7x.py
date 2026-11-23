import subprocess, os
os.chdir(r"F:\Work\Leonida")
f = "runtime/src/main.cpp"
t = open(f, encoding="utf-8").read()
i = t.find("<<<<<<<"); e = t.find(">>>>>>> pr/7111", i); m = t.find("=======", i)
t = t[:i] + '#include "host_cpu.h"\n#include "cpu_emulation.h"' + t[e+len(">>>>>>> pr/7111"):]
open(f,"w",encoding="utf-8",newline="\n").write(t)
assert "<<<<<<<" not in t
print("cpu_emulation.h exists:", os.path.exists("runtime/src/cpu_emulation.h"))
subprocess.run(["git","add","-A"], capture_output=True)
r = subprocess.run(["git","commit","-q","-m","stack: merge PR #7111 (Zen2 CPU emulation) onto new main"], capture_output=True, text=True)
print("committed" if r.returncode==0 else r.stderr[:300])
r = subprocess.run(["git","status","--porcelain"], capture_output=True, text=True)
print("clean:", not r.stdout.strip())
r = subprocess.run(["git","log","--oneline","origin/main..HEAD"], capture_output=True, text=True)
print(r.stdout.count("\n"), "commits ahead of main")
