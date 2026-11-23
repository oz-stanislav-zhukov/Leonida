$p = Start-Process -FilePath 'F:\Work\Leonida\runtime\build\gta6rt.exe' `
  -ArgumentList @('--run', 'F:\GTA6\GTA6.Xbox.v1.0.2.1-CYBERLEEK\game_scarlett_master.exe', '--game-dir', 'F:\GTA6\GTA6.Xbox.v1.0.2.1-CYBERLEEK') `
  -WorkingDirectory 'F:\Work\Leonida' -NoNewWindow -PassThru `
  -RedirectStandardOutput 'F:\Work\Leonida\unsloth-tmp\game_run_621.log' `
  -RedirectStandardError 'F:\Work\Leonida\unsloth-tmp\game_run_621.err.log'
if (-not $p.WaitForExit(600000)) { $p.Kill(); Write-Host 'GAME_TIMEOUT_KILLED_AFTER_10MIN' } else { Write-Host ('GAME_EXIT=' + $p.ExitCode) }
