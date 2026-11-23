<!-- replaced 2026-11-23 -->
[2026-11-22] <leonida-build-env> Стас просил всегда использовать `--work F:\Leonida` при запуске make_game.py (рабочие файлы на F:, там ~650 ГБ свободно). Зависимости установлены 22.11.2026: .NET 8 SDK → C:\Users\ozlig\AppData\Local\Microsoft\dotnet, ffmpeg 9.1 → F:\tools\ffmpeg\bin, Vulkan SDK → F:\tools\VulkanSDK, llvm-mc → F:\tools\llvm-mc (LLVM_MC выставляется скриптом). Всё добавлено в пользовательский PATH.

<!-- replaced 2026-11-23 -->
[2026-11-23] <leonida-build-env> После пулла до 561b65a8 (23.11.2026) make_game.py требует Python 3.12+. Установлен Python 3.13 в C:\Python313\python.exe (в PATH он после 3.11, поэтому `python` по-прежнему указывает на 3.11 — использовать полный путь C:\Python313\python.exe или py -3.13). compileall + --help с 3.13 прошли OK.

<!-- replaced 2026-11-23 -->
[2026-11-23] <leonida-build-env> Стас просил всегда использовать `--work F:\Leonida` при запуске make_game.py (рабочие файлы на F:, там ~650 ГБ свободно). Зависимости: .NET 8 SDK → C:\Users\ozlig\AppData\Local\Microsoft\dotnet, ffmpeg 9.1 → F:\tools\ffmpeg\bin, Vulkan SDK 1.4.363.1 → F:\tools\VulkanSDK, llvm-mc 21.1.8 → F:\tools\llvm-mc (LLVM_MC выставляется скриптом), всё в пользовательском PATH. С 23.11.2026 (коммит 561b65a8) make_game.py требует Python 3.12+: использовать C:\Python313\python.exe (в PATH он после 3.11, поэтому голый `python` = 3.11; альтернатива py -3.13). compileall + --help с 3.13 прошли OK.

<!-- replaced 2026-11-23 -->
[2026-11-23] <git sync> The repository was fast-forwarded to commit 49475796, bringing significant updates to the D3D12-to-Vulkan runtime layer and make_game.py.

<!-- replaced 2026-11-23 -->
[2026-11-23] <git sync> Репозиторий обновлён до коммита 1cc6e3c12 (120 новых коммитов), фокус на рантайме и новых тестах.

<!-- replaced 2026-11-23 -->
[2026-11-23] <regression> В test_float_nan_fract.py (v_med3_f32) обнаружена новая регрессия, текущий коммит 1cc6e3c12 не содержит исправления.

<!-- replaced 2026-11-23 -->
[2026-11-23] <git sync> Репозиторий обновлён до коммита 8575d36ed (fast-forward, 87 новых коммитов).

<!-- replaced 2026-11-24 -->
[2026-11-24] <posting-rule> Перед ЛЮБЫМ постом/комментарием в issue или PR Leonida — сначала поискать похожие открытые issues и PR (gh issue list --search / gh pr list), чтобы не дублировать. Правило Стаса от 24.11.2026.

<!-- replaced 2026-11-24 -->
[2026-11-24] <posting-rule> Стас разрешил (24.11.2026): создавать issues/PR Leonida, если такого решения ещё нет, ИЛИ отвечать в issues своими итогами/статистикой (например GPU-цифры в #166). ОБЯЗАТЕЛЬНО: (1) перед любым постом поискать похожие открытые issues/PR (антидубли), (2) всегда информировать Стаса о том, что и куда постили, (3) посты делать от имени qwen, без имени ассистента.

