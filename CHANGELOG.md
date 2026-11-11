# Changelog

## 1.3.0

- Added a Simplified Chinese interface across the app, installer and uninstaller.
- Added radial stick mapping that compensates the measured 15/128 onset at 0% additional deadzone and avoids cross-shaped diagonal trapping.
- Added full localization coverage checks to the Windows build workflow.

## 1.0.3

- Fixed Bluetooth rumble on Windows 11 24H2 (build 26100) by probing its GATT device interface IID, with fallback to the existing interface (#3).
- Added a Windows build workflow that compiles the driver and installers, runs tests and saves build artifacts. Install the full Setup package to apply the driver fix; the app-only updater cannot replace the driver.

## 1.0.2

- Improved vibration gain, live adjustments and full-range tests.
- Added slider track clicks, update notifications and an app-only updater.

## 1.0.1

- Fixed incomplete translations in English, French and German.

## 1.0.0

- Initial release.
