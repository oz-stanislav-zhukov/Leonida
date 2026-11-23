$old = '32.0.16.1742'
$deadline = (Get-Date).AddHours(2)
while ((Get-Date) -lt $deadline) {
    Start-Sleep -Seconds 60
    $v = (Get-CimInstance Win32_VideoController).DriverVersion
    if ($v -ne $old) {
        Write-Output "DRIVER UPDATED: $old -> $v"
        break
    }
    Write-Output ('heartbeat {0:HH:mm} - still {1}' -f (Get-Date), $v)
}
Write-Output ('final: ' + (Get-CimInstance Win32_VideoController).DriverVersion)
