$ErrorActionPreference = "Stop"
New-Item -ItemType Directory -Force F:\tmp | Out-Null
Write-Host "downloading Vulkan SDK via curl..."
curl.exe -L --fail -o F:\tmp\vulkan_sdk.exe 'https://sdk.lunarg.com/sdk/download/latest/windows/vulkan_sdk.exe?Human=true'
if ($LASTEXITCODE -ne 0) { throw "curl failed ($LASTEXITCODE)" }
Write-Host ("downloaded " + [math]::Round((Get-Item F:\tmp\vulkan_sdk.exe).Length / 1MB) + " MB")
$p = Start-Process F:\tmp\vulkan_sdk.exe -ArgumentList '/S', '/DIR=F:\tools\VulkanSDK' -Wait -PassThru
Write-Host ("installer exit: " + $p.ExitCode)
if (Test-Path F:\tools\VulkanSDK\Bin\spirv-val.exe) { Write-Host "SPIRV tools OK" } else { Write-Host "INSTALL DIR MISSING OR INCOMPLETE" }
