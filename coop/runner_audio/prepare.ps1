# Runner-only activation of the existing Windows audio services.
$ErrorActionPreference = 'Stop'
foreach ($name in @('AudioEndpointBuilder', 'Audiosrv')) {
    $service = Get-Service -Name $name -ErrorAction SilentlyContinue
    if ($null -eq $service) {
        Write-Output "BO_RUNNER_AUDIO_SERVICE_${name}_MISSING"
        continue
    }
    try {
        if ($service.Status -ne 'Running') {
            Set-Service -Name $name -StartupType Manual
            Start-Service -Name $name
            $service.WaitForStatus('Running', [TimeSpan]::FromSeconds(20))
        }
        Write-Output "BO_RUNNER_AUDIO_SERVICE_${name}_RUNNING"
    } catch {
        Write-Output "BO_RUNNER_AUDIO_SERVICE_${name}_UNAVAILABLE"
    }
}
