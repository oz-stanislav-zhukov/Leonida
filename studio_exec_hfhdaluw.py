import os, subprocess
os.chdir(r"F:\Work\Leonida")
f = "runtime/test/xs_proxy_mock_test.cpp"
t = open(f, encoding="utf-8").read()
t = t.replace("command_list, heap };/7021", "command_list, heap };")
open(f,"w",encoding="utf-8",newline="\n").write(t)
assert "/7021" not in t and "<<<<<<<" not in t
print("check_placed_resource_x in main:", "if (check_placed_resource_x()) return 1;" in t)
subprocess.run(["git","add","-A"], capture_output=True)
r = subprocess.run(["git","commit","-q","-m","stack: merge PR #7066 (hammokko boot stack)"], capture_output=True, text=True)
print("committed" if r.returncode==0 else r.stderr[:300])
