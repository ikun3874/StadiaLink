<div align="center">
  <img src="app/assets/readme-logo.svg" width="112" height="112" alt="StadiaLink 标志">
  <h1>StadiaLink</h1>
  <p><strong>你的 Stadia，由你设定。</strong></p>
  <p>适用于 Windows 11 x64 的 Stadia 手柄配置工具 · 版本 1.3.0</p>
  <p><a href="https://github.com/minysito/StadiaLink/releases/latest"><strong>⬇ 下载</strong></a> · <a href="https://github.com/minysito/StadiaLink/issues">反馈问题</a> · <a href="LICENSE">GPL-3.0</a></p>
  <p>Windows 11 x64 · XInput · USB 与蓝牙</p>
  <p><a href="README.md">Español</a> &nbsp; | &nbsp; <a href="README.en.md">English</a> &nbsp; | &nbsp; <a href="README.fr.md">Français</a> &nbsp; | &nbsp; <a href="README.de.md">Deutsch</a> &nbsp; | &nbsp; <strong>简体中文</strong></p>
</div>

---

## 功能

- 通过 USB 和蓝牙连接手柄，并输出兼容 XInput 的输入。
- 自定义按键映射、快捷键和按键说话。
- 调整摇杆灵敏度、死区和扳机。
- 分别测试两个振动马达，也可持续振动并手动停止。
- 导入和导出配置文件。
- 显示电量，支持通知、系统托盘和 Windows 启动项。
- 界面支持西班牙语、英语、法语、德语和简体中文。

## 安装

从 [Releases](https://github.com/minysito/StadiaLink/releases/latest) 下载 `StadiaLink.Setup.exe` 并运行。安装驱动时需要管理员权限。安装程序包含所需组件，可离线运行。请在 Windows 蓝牙设置中配对手柄，或通过 USB 连接。

蓝牙连接要求手柄已安装蓝牙固件；StadiaLink 不会修改手柄固件。程序使用 .NET Framework 4.8 和 Windows 组件，不需要第三方手柄模拟器或额外运行时。

可将下载文件的 SHA256 与同一版本中的 `SHA256SUMS.txt` 比较：

```powershell
Get-FileHash .\StadiaLink.Setup.exe -Algorithm SHA256
```

## 从源码构建

完整构建步骤和依赖要求请参阅英文说明中的「Build from scratch」部分。
