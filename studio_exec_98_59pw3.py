import os, subprocess
os.chdir(r"F:\Work\Leonida")
f = "runtime/shims/d3d12_com.cpp"
t = open(f, encoding="utf-8").read()
i = t.find("<<<<<<<"); e = t.find(">>>>>>> pr/7330", i); m = t.find("=======", i)
head = t[i+len("<<<<<<< HEAD\n"):m].rstrip("\n")
new = """        if (kind == leonida::IndirectKind::dispatch_mesh) {
            // Not replayed yet (#3777 replays it as vkCmdDrawMeshTasksIndirectEXT): nothing is recorded, and the
            // first such call says so. The list stays valid.
            static std::atomic<bool> logged{false};
            if (!logged.exchange(true, std::memory_order_relaxed))
                std::fprintf(stderr, "[rt] d3d12: ExecuteIndirect with a DISPATCH_MESH signature is not replayed yet; "
                                     "nothing was recorded\\n");
            return;
        }
""" + head
t = t[:i] + new + t[e+len(">>>>>>> pr/7330"):]
open(f,"w",encoding="utf-8",newline="\n").write(t)
assert "<<<<<<<" not in t
# restore the accidentally deleted tracked file
subprocess.run(["git","checkout","--","studio_exec_lsbdstld.py"], capture_output=True)
subprocess.run(["git","add","-A"], capture_output=True)
r = subprocess.run(["git","commit","-q","-m","stack: merge PR #7330 (DISPATCH_MESH command signature) onto new main"], capture_output=True, text=True)
print("committed" if r.returncode==0 else r.stderr[:300])
r = subprocess.run(["git","status","--porcelain"], capture_output=True, text=True)
print("clean:", not r.stdout.strip())
print(try_merge := None)
