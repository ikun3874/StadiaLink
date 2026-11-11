$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$out=Join-Path $root 'release'
$scratch=Join-Path $root '.build/app'
New-Item -ItemType Directory -Force $out,$scratch | Out-Null
$vswhere="${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vs=& $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if(-not $vs){throw 'MSVC is needed to build the native battery reader'}
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
& cl /nologo /W4 /O2 /MT /LD /EHsc /std:c++17 /DUNICODE /D_UNICODE /D_WIN32_WINNT=0x0A00 /Fo"$scratch\native-device.obj" /Fe"$out\StadiaDevice.dll" "$PSScriptRoot\native-device.cpp" /link /NOIMPLIB /NOEXP SetupAPI.lib BluetoothApis.lib Winusb.lib
if($LASTEXITCODE){throw 'Native battery reader compilation failed'}
& cl /nologo /W4 /O2 /MT /EHsc /std:c++17 /DUNICODE /D_UNICODE /D_WIN32_WINNT=0x0A00 /DBATTERY_PROBE /Fo"$scratch\battery-probe.obj" /Fe"$scratch\battery-probe.exe" "$PSScriptRoot\native-device.cpp" /link SetupAPI.lib BluetoothApis.lib Winusb.lib
if($LASTEXITCODE){throw 'Battery probe compilation failed'}

