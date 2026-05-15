# XiahGameEngine (新侠道 - 游戏引擎库)

## 项目简介
XiahGameEngine 是为《新侠道》(Xiah) 客户端提供底层支持的定制游戏引擎。它将图形渲染、物理碰撞、数学运算库、UI 框架（NEWINTERFACE）以及文件资源打包读取系统（XPK/XPC）等功能进行了深度封装，是整个客户端画面呈现与正常运作的基石。

## 有啥用 (核心功能)
- **图形与渲染**: 基于 DirectX，封装了 Camera（摄像机）、Terrain（地形）、SkyBox（天空盒）及复杂的光照系统。
- **物理与数学**: 提供专为 3D 游戏优化的向量/矩阵运算，以及角色、建筑间的物理碰撞检测机制。
- **自研 UI 系统**: 包含一套完善的事件驱动型 UI 控件树（如 CUIButton, CUITreeCtrl, CUIProgressCtrl 等）。
- **粒子与特效**: 实现技能释放时的华丽特效、天气系统（如雨雪物理表现）、武器残影等。
- **资产加载**: 高效读取自定义的二进制高压缩包格式，并集成 FMOD 等音频播放。

## 编译依赖
- **操作系统**: Windows
- **开发工具**: Visual Studio (一般推荐保留与原版工程匹配的平台工具集，如 v140 或 v142)。
- **图形 API**: **DirectX SDK (June 2010)** 或更早版本，代码中大量依赖传统的 `d3d9.h` / `d3dx9.h`。
- **音频库**: FMOD (需自备匹配的头文件和 `.lib`/`.dll` 库文件)。

## 如何编译
1. 安装必备的 **DirectX SDK**。
2. 打开 Visual Studio，配置全局或本项目专属的“包含目录”和“库目录”，将 DX SDK 的 Include 和 Lib 路径加入其中。
3. 双击打开 `XiahGameEngine.sln` 或 `XiahGameEngine.vcxproj`。
4. 在上方工具栏选择对应的编译架构（通常是 `x86`）与配置（`Debug` 或 `Release`）。
5. 点击 **生成解决方案 (Build Solution)**。
6. 编译完成后，将会生成静态库文件 `.lib` (或动态链接库 `.dll`)。客户端项目在编译时需要链接此产物。
