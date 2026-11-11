$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$out=Join-Path $root '.build/app'
New-Item -ItemType Directory -Force $out | Out-Null
$vswhere="${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vs=& $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if(-not $vs){throw 'MSVC build tools not found'}
$shortPathType='StadiaBuildShortPath' -as [type]
if(-not $shortPathType){Add-Type -TypeDefinition @'
using System.Text;
using System.Runtime.InteropServices;
public static class StadiaBuildShortPath {
    [DllImport("kernel32.dll", CharSet=CharSet.Unicode)]
    public static extern uint GetShortPathName(string path, StringBuilder shortPath, uint size);
}
'@}
$shortBat=New-Object Text.StringBuilder 1024
if(-not [StadiaBuildShortPath]::GetShortPathName((Join-Path $vs 'VC\Auxiliary\Build\vcvars64.bat'),$shortBat,$shortBat.Capacity)){throw 'Could not resolve the MSVC environment script path'}
cmd /c "call $($shortBat.ToString()) >nul 2>&1 && set" | ForEach-Object {if($_ -match '^([^=]+)=(.*)$'){Set-Item "env:$($Matches[1])" $Matches[2]}}
& cl /nologo /W4 /O2 /MT /EHsc /std:c++17 /Fo"$out\configuration-tests.obj" /Fe"$out\configuration-tests.exe" "$PSScriptRoot\configuration-tests.cpp"
if($LASTEXITCODE){throw 'Configuration tests compilation failed'}
& "$out\configuration-tests.exe" | Tee-Object "$out\translation-tests.txt"
if($LASTEXITCODE){throw 'Translation tests failed'}
$process=Start-Process "$root\release\StadiaLink.exe" -ArgumentList '--self-test',('"'+[IO.Path]::GetFullPath("$out\model-tests.txt")+'"') -PassThru -Wait -WindowStyle Hidden
if($process.ExitCode){throw 'Application model tests failed'}
Get-Content "$out\model-tests.txt"
$liveResult=[IO.Path]::GetFullPath("$out\live-motor-tests.txt")
$process=Start-Process "$root\release\StadiaLink.exe" -ArgumentList '--motor-live-test',('"'+$liveResult+'"') -PassThru -Wait -WindowStyle Hidden
if($process.ExitCode -or -not (Test-Path $liveResult)){throw 'Live vibration slider tests failed'}
Get-Content $liveResult
