# GPU Temperature Monitor (D3D Visualization Tool)

## Features

1. **Visualize GPU Temperature Changes**  
   Real-time visualization of GPU temperature changes over a period of time.
   
2. **Detailed GPU Temperature Information**  
   - View standard GPU temperature.
   - View GPU **Hotspot Temperature** and **Memory Temperature**.  
   These features are not available in most mainstream tools like **NVIDIA GeForce Experience** or the **Windows Task Manager**.  
   This is achieved through **NVAPI's undocumented interface**. Currently, only **AIDA64** provides similar data.

   - **Red line**: GPU temperature
   - **Cyan line**: GPU hotspot temperature
   - **Yellow line**: GPU memory temperature

   Temperature range: 0°C / 25°C / 50°C / 75°C.

3. **DirectX 3D Rendering**  
   The tool uses DirectX to render 2D graphs, allowing it to **transparently overlay on top of any game** without affecting gameplay.

4. **Toggle Display**  
   You can toggle the display ON/OFF using the keyboard shortcut **CTRL+SHIFT+P**.

5. **System Tray Icon**  
   The tool runs in the system tray, and you can **exit the program completely** via the tray icon.

## Limitations

- Currently, the tool only supports **NVIDIA GPUs**. It will not work with integrated or AMD GPUs.
- If you have multiple NVIDIA GPUs, the tool will display information for **the last GPU** detected by the system.

## Tested Environment

- **OS**: Windows 11
- **CPU**: Intel
- **GPU**: NVIDIA GeForce RTX 4090

## License

This project is licensed under the **GPL** license.

---

# GPU 温度监测工具 (D3D 可视化)

## 特色

1. **可视化查看GPU温度变化**  
   实时可视化查看一段时间内的 GPU 温度变化。

2. **详细的GPU温度信息**  
   - 查看 GPU 的常规温度。
   - 查看 GPU **热点温度** 和 **显存温度**。  
     目前市场上常规工具（如 **NVIDIA GeForce Experience** 或 **Windows 资源管理器**）无法查看这些数据。  
     本工具通过 **NVAPI 的未公开接口** 实现。目前仅 **AIDA64** 提供类似功能。

   - **红色曲线**：GPU 温度
   - **青色曲线**：GPU 热点温度
   - **黄色曲线**：GPU 显存温度

   温度刻度：0°C / 25°C / 50°C / 75°C。

3. **DirectX 3D 渲染**  
   该工具使用 DirectX 渲染 2D 图形，能够 **透明悬浮在游戏界面上**，不影响正常操作。

4. **切换显示**  
   可以通过快捷键 **CTRL+SHIFT+P** 随时切换显示开关。

5. **托盘图标**  
   该工具在系统托盘中运行，可以通过托盘图标 **彻底退出程序**。

## 使用限制

- 目前仅支持 **NVIDIA 显卡**，不适用于集成显卡或 AMD 显卡。
- 如果您的系统有多张 NVIDIA 显卡，默认只显示**最后一张显卡**的温度信息。

## 测试环境

- **操作系统**: Windows 11
- **处理器**: Intel
- **显卡**: NVIDIA GeForce RTX 4090

## 开源协议

本项目使用 **GPL** 协议进行开源。
