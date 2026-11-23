[2026-11-22] <о-проекте> Leonida is a static recompilation project. The Xbox Series CPU is x86-64 like a PC, so the game's code is loaded

[2026-11-22] <структура> папки верхнего уровня: assets/, docs/, game/, runtime/, scripts/, tests/, third_party/, tools/, viewer/

[2026-11-22] <gta6 release> The game released on PS5 and Xbox Series X|S on November 19, 2026, with no PC version available yet.

[2026-11-22] <user preference> The user is highly concerned about phishing and requires the assistant to manually verify external links before sharing them.

[2026-11-23] <vulkan sdk> The duplicate Vulkan SDK installer in C:\VulkanSDK was confirmed harmless and removed; the active SDK remains at F:\tools\VulkanSDK.

[2026-11-23] <leonida-build-env> Стас просил всегда использовать `--work F:\Leonida` при запуске make_game.py (рабочие файлы на F:, там ~650 ГБ свободно). Зависимости: .NET 8 SDK → C:\Users\ozlig\AppData\Local\Microsoft\dotnet, ffmpeg 9.1 → F:\tools\ffmpeg\bin, Vulkan SDK 1.4.363.1 → F:\tools\VulkanSDK, llvm-mc 21.1.8 → F:\tools\llvm-mc (LLVM_MC выставляется скриптом), всё в пользовательском PATH. С 23.11.2026 (коммит 561b65a8) make_game.py требует Python 3.12+: в начале пользовательского PATH добавлен C:\Python313 (через реестр, не setx; системный PATH тронут не был, там 3.11 перед 3.13), поэтому в новых окнах голый `python` = 3.13; в старых процессах/сессиях до перезапуска — всё ещё 3.11, надёжнее явный C:\Python313\python.exe или py -3.13. compileall + --help с 3.13 прошли OK.

[2026-11-23] <python path> В пользовательский PATH добавлен C:\Python313 в начало, чтобы голый python указывал на 3.13 без правок системного PATH.

[2026-11-23] <regression> Обнаружена новая регрессия в test_image_atomics_guard.py (47 падений, OpImageTexelPointer in unguarded block).

[2026-11-23] <test runner> В AGENTS.md появился официальный раннер scripts/run_semantic_tests.py (по файлу на ядро) и новый scripts/dev.py.

[2026-11-23] <leonida issue 166> User will write the comment themselves; assistant only provides the text with real GPU/driver stats.

[2026-11-23] <makegame.py resume> User asked if restarting with more jobs resumes from the same point; assistant is investigating the behavior.

[2026-11-23] <git sync> Repository fetched 9 new commits on main (up to dde82f27e) but intentionally not merged to avoid disrupting the active build.

[2026-11-24] <leonida build> Сборка статической рекомпиляции остановлена на ~56% из-за медленной компиляции рейтрейсинг-шейдеров (5-8 мин/батч).

[2026-11-24] <leonida strategy> Принято решение перейти на динамический транслятор (DirectX→Vulkan) для быстрого запуска игры.

[2026-11-24] <leonida runtime> Идёт пересборка рантайма со шимом GameInput+SDL3 для запуска через динамический транслятор.

[2026-11-24] <driver> NVIDIA 621.24 установлен, Vulkan работает (ps_ops 8/8), но переменная LLVM_MC сбрасывается после перезагрузки.

[2026-11-24] <tests> 7 тестов CTest упали (root_signature_oracle, d3d12_vulkan_device, d3d12_close_parity, d3d12_execute_indirect_validation, d3d12_depth_resolve, d3d12_pso_validation, d3d12_set_descriptor_heaps).

[2026-11-24] <gpu tests> 176 из 211 тестов GPU-проверок прошли успешно, 35 пропущено.

[2026-11-24] <gta6 blocker> DStorageGetFactory (xcrdapi.dll) is the current blocker; community PR #8461 implements it and is being built/tested.

[2026-11-24] <gta6 runtime> Game launch via dynamic translator (D3D12→Vulkan) passes TLS-crash and XErrorReport, but stops at DStorageGetFactory.

[2026-11-24] <leonida issue 9522> Coordination issue for DStorageGetFactory blocker exists; assistant is tracking it per Stas's duplicate-check rule.

[2026-11-24] <posting-rule> Правило постинга Leonida (обновлено 24.11.2026): (1) проверять issues/PR на дубли перед любым постом; (2) можно создавать issues/PR и писать нормальные фиксы, если решения ещё нет; (3) МОЖНО писать комментарии в issues со своими итогами/статистикой; (4) ОБЯЗАТЕЛЬНО информировать Стаса через TELEGRAM (notify) о каждом посте: что, куда, с ссылкой; (5) в #166 НЕ создавать новые записи для той же GPU+драйвер — ОБНОВЛЯТЬ свой комментарий https://github.com/ItsNotRexed/Leonida/issues/166#issuecomment-7114659588; (6) посты от имени qwen, без имени ассистента, без игровых данных.
