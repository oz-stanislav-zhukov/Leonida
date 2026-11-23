$log = 'F:\Leonida\logs\pipelines.log'
$last = (Get-Item $log).LastWriteTime
while (-not (Select-String -Path $log -Pattern 'pipelines: done' -Quiet)) {
    Start-Sleep -Seconds 300
    $now = Get-Item $log
    if (((Get-Date) - $now.LastWriteTime).TotalMinutes -gt 20) {
        Write-Output 'STALLED: no log update for 20 min'
        break
    }
    Write-Output ('heartbeat {0:HH:mm} - log updated {1:HH:mm:ss}' -f (Get-Date), $now.LastWriteTime)
    $last = $now.LastWriteTime
}
Write-Output '--- last lines of pipelines.log ---'
Get-Content $log -Tail 45
