@echo off
cd /d F:\Work\Leonida
git apply --check unsloth-tmp\prstack\6964.diff >nul 2>&1
if not errorlevel 1 (git apply unsloth-tmp\prstack\6964.diff >nul 2>&1 && echo 6964=APPLIED || echo 6964=APPLY_FAILED)
else (git apply -R --check unsloth-tmp\prstack\6964.diff >nul 2>&1 && echo 6964=ALREADY || echo 6964=CONFLICT)
git apply --check unsloth-tmp\prstack\6954.diff >nul 2>&1
if not errorlevel 1 (git apply unsloth-tmp\prstack\6954.diff >nul 2>&1 && echo 6954=APPLIED || echo 6954=APPLY_FAILED)
else (git apply -R --check unsloth-tmp\prstack\6954.diff >nul 2>&1 && echo 6954=ALREADY || echo 6954=CONFLICT)
git apply --check unsloth-tmp\prstack\6953.diff >nul 2>&1
if not errorlevel 1 (git apply unsloth-tmp\prstack\6953.diff >nul 2>&1 && echo 6953=APPLIED || echo 6953=APPLY_FAILED)
else (git apply -R --check unsloth-tmp\prstack\6953.diff >nul 2>&1 && echo 6953=ALREADY || echo 6953=CONFLICT)
git apply --check unsloth-tmp\prstack\7075.diff >nul 2>&1
if not errorlevel 1 (git apply unsloth-tmp\prstack\7075.diff >nul 2>&1 && echo 7075=APPLIED || echo 7075=APPLY_FAILED)
else (git apply -R --check unsloth-tmp\prstack\7075.diff >nul 2>&1 && echo 7075=ALREADY || echo 7075=CONFLICT)
git apply --check unsloth-tmp\prstack\7021.diff >nul 2>&1
if not errorlevel 1 (git apply unsloth-tmp\prstack\7021.diff >nul 2>&1 && echo 7021=APPLIED || echo 7021=APPLY_FAILED)
else (git apply -R --check unsloth-tmp\prstack\7021.diff >nul 2>&1 && echo 7021=ALREADY || echo 7021=CONFLICT)
git apply --check unsloth-tmp\prstack\7066.diff >nul 2>&1
if not errorlevel 1 (git apply unsloth-tmp\prstack\7066.diff >nul 2>&1 && echo 7066=APPLIED || echo 7066=APPLY_FAILED)
else (git apply -R --check unsloth-tmp\prstack\7066.diff >nul 2>&1 && echo 7066=ALREADY || echo 7066=CONFLICT)
git apply --check unsloth-tmp\prstack\7052.diff >nul 2>&1
if not errorlevel 1 (git apply unsloth-tmp\prstack\7052.diff >nul 2>&1 && echo 7052=APPLIED || echo 7052=APPLY_FAILED)
else (git apply -R --check unsloth-tmp\prstack\7052.diff >nul 2>&1 && echo 7052=ALREADY || echo 7052=CONFLICT)
git apply --check unsloth-tmp\prstack\7110.diff >nul 2>&1
if not errorlevel 1 (git apply unsloth-tmp\prstack\7110.diff >nul 2>&1 && echo 7110=APPLIED || echo 7110=APPLY_FAILED)
else (git apply -R --check unsloth-tmp\prstack\7110.diff >nul 2>&1 && echo 7110=ALREADY || echo 7110=CONFLICT)
git apply --check unsloth-tmp\prstack\8278.diff >nul 2>&1
if not errorlevel 1 (git apply unsloth-tmp\prstack\8278.diff >nul 2>&1 && echo 8278=APPLIED || echo 8278=APPLY_FAILED)
else (git apply -R --check unsloth-tmp\prstack\8278.diff >nul 2>&1 && echo 8278=ALREADY || echo 8278=CONFLICT)
git apply --check unsloth-tmp\prstack\7115.diff >nul 2>&1
if not errorlevel 1 (git apply unsloth-tmp\prstack\7115.diff >nul 2>&1 && echo 7115=APPLIED || echo 7115=APPLY_FAILED)
else (git apply -R --check unsloth-tmp\prstack\7115.diff >nul 2>&1 && echo 7115=ALREADY || echo 7115=CONFLICT)
git apply --check unsloth-tmp\prstack\7163.diff >nul 2>&1
if not errorlevel 1 (git apply unsloth-tmp\prstack\7163.diff >nul 2>&1 && echo 7163=APPLIED || echo 7163=APPLY_FAILED)
else (git apply -R --check unsloth-tmp\prstack\7163.diff >nul 2>&1 && echo 7163=ALREADY || echo 7163=CONFLICT)
git apply --check unsloth-tmp\prstack\7201.diff >nul 2>&1
if not errorlevel 1 (git apply unsloth-tmp\prstack\7201.diff >nul 2>&1 && echo 7201=APPLIED || echo 7201=APPLY_FAILED)
else (git apply -R --check unsloth-tmp\prstack\7201.diff >nul 2>&1 && echo 7201=ALREADY || echo 7201=CONFLICT)
git apply --check unsloth-tmp\prstack\7330.diff >nul 2>&1
if not errorlevel 1 (git apply unsloth-tmp\prstack\7330.diff >nul 2>&1 && echo 7330=APPLIED || echo 7330=APPLY_FAILED)
else (git apply -R --check unsloth-tmp\prstack\7330.diff >nul 2>&1 && echo 7330=ALREADY || echo 7330=CONFLICT)
git apply --check unsloth-tmp\prstack\7490.diff >nul 2>&1
if not errorlevel 1 (git apply unsloth-tmp\prstack\7490.diff >nul 2>&1 && echo 7490=APPLIED || echo 7490=APPLY_FAILED)
else (git apply -R --check unsloth-tmp\prstack\7490.diff >nul 2>&1 && echo 7490=ALREADY || echo 7490=CONFLICT)
git apply --check unsloth-tmp\prstack\7111.diff >nul 2>&1
if not errorlevel 1 (git apply unsloth-tmp\prstack\7111.diff >nul 2>&1 && echo 7111=APPLIED || echo 7111=APPLY_FAILED)
else (git apply -R --check unsloth-tmp\prstack\7111.diff >nul 2>&1 && echo 7111=ALREADY || echo 7111=CONFLICT)