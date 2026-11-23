import os, subprocess
os.chdir(r"F:\Work\Leonida")
f = "runtime/CMakeLists.txt"
t = open(f, encoding="utf-8").read()
i = t.find("<<<<<<<")
if i >= 0:
    e = t.find(">>>>>>>", i)
    m = t.find("=======", i)
    head = t[i+len("<<<<<<< HEAD\n"):m].rstrip("\n")
    pr = t[m+7:e].rstrip("\n")
    end_marker = t[e:t.find("\n", e)]
    t = t[:i] + (head + "\n" + pr if head else pr) + t[e+len(end_marker):]
    open(f,"w",encoding="utf-8",newline="\n").write(t)
print("markers left:", t.count("<<<<<<<"))
print("xapu in cmake:", "shims/xapu_chad.cpp" in t)
subprocess.run(["git","add","-A"], capture_output=True)
r = subprocess.run(["git","commit","-q","-m","shims: xapu/chad audio shims (port of #6981 onto new main; AcpHal stays the upstream one)"], capture_output=True, text=True)
print("committed" if r.returncode==0 else r.stderr[:300])
r = subprocess.run(["git","status","--porcelain"], capture_output=True, text=True)
print("clean:", not r.stdout.strip())
r = subprocess.run(["git","log","--oneline","origin/main..HEAD"], capture_output=True, text=True)
print(r.stdout.count("\n"), "commits ahead of origin/main")
