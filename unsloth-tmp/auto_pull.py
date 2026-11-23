#!/usr/bin/env python3
"""Pull Leonida main every 10 minutes (fast-forward only). Prints one line per cycle."""
import subprocess
import time

REPO = r"F:\Work\Leonida"
INTERVAL = 600

while True:
    stamp = time.strftime("%Y-%m-%d %H:%M:%S")
    try:
        p = subprocess.run(
            ["git", "pull", "--ff-only"],
            cwd=REPO, capture_output=True, text=True, timeout=300,
        )
        lines = [l for l in (p.stdout + p.stderr).strip().splitlines() if l.strip()]
        head = lines[0][:160] if lines else "ok"
        if p.returncode == 0 and head.startswith("Already up to date"):
            print(stamp, "| up-to-date", flush=True)
        elif p.returncode == 0:
            print(stamp, "| PULLED |", head, flush=True)
        else:
            print(stamp, "| ERROR rc=%d |" % p.returncode, head, flush=True)
    except Exception as e:  # noqa: BLE001 - keep the loop alive
        print(stamp, "| EXCEPTION |", str(e)[:160], flush=True)
    time.sleep(INTERVAL)
