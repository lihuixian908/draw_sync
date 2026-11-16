# DrawSync · 联机你画我猜

DrawSync 是用 C++17 和 Qt Widgets 编写的桌面客户端。玩家在同一个房间里轮流画画、猜词，通过 WebSocket 同步笔迹和聊天消息。房主负责开始游戏和回合，客户端在本地用 SQLite 记录和显示排行榜。

**联机前需要启动一个兼容的 DrawSync WebSocket 服务端。** 本仓库包含桌面客户端和浏览器调试页。房间、画手分配、题词和猜词结果由服务端处理。桌面端预填的 `ws://127.0.0.1:9000` 指向本机，可在界面上改成实际服务地址。

## 功能

- 创建、刷新和加入房间，由房主控制游戏进度。
- 在回合中同步画笔颜色、粗细、橡皮擦、撤回和清空操作。
- 画手查看题词，其他玩家在 60 秒倒计时内提交猜测。
- 使用独立聊天窗口交流，并查看客户端本地排行榜。

## 构建和运行

以下步骤以 Windows、Qt 6 和 MinGW 64 位为例。仓库原有构建配置使用 Qt 6.10.0 和 MinGW 13.1。请使用与 Qt 套件匹配的 C++17 编译器，以及 CMake（工程声明的下限为 3.16）。Qt 组件包括 Widgets、WebSockets、LinguistTools、Svg、SvgWidgets、Xml 和 Sql，运行时需要 SQLite 驱动（`QSQLITE`）。使用 Ninja 生成器时还需要 Ninja。

### Qt Creator

打开根目录的 `CMakeLists.txt`，选择匹配的 Qt 6 Desktop Kit，配置项目，然后运行 `PaintEngineDemo`。

### PowerShell

将示例中的 Qt 和工具目录改成你的安装路径。请逐条执行构建命令，确认上一步成功后再继续。

```powershell
git clone https://github.com/lihuixian908/draw_sync.git
cd draw_sync

$qt = 'C:\Qt\6.10.0\mingw_64'
$env:Path = "$qt\bin;C:\Qt\Tools\mingw1310_64\bin;C:\Qt\Tools\Ninja;$env:Path"

cmake -S . -B build/local -G Ninja -DCMAKE_BUILD_TYPE=Release "-DCMAKE_PREFIX_PATH=$qt"
cmake --build build/local
.\build\local\PaintEngineDemo.exe
```

程序在当前工作目录创建 `game_process.db`，启动时重建计分表。在同一台机器上测试多个客户端时，从不同的可写工作目录启动每个实例，让它们使用各自的数据库。

## 双人联机验证

1. 启动兼容服务端。打开两个客户端窗口，填入同一个服务器地址和不同的玩家名，分别点击「连接」。
2. 玩家 A 输入房间名并点击「创建房间」。玩家 B 点击「刷新房间列表」，选择 A 创建的房间加入。
3. 房主点击「开始游戏」，再点击「开始回合」。服务端分配身份后，确认画手能看到题词，猜词者收到猜词弹窗。
4. 在画手端画几笔，再试一次撤回和清空。确认猜词端的画布同步更新。随后提交猜测，并在两个窗口之间互发聊天消息。
5. 观察回合结束后的界面变化，点击「排行榜」查看本地计分。

可直接在浏览器打开 [WebSocket 调试页](<drawsync_test(1).html>)。先把页面预填的服务器地址替换成可访问的地址，再测试连接、房间、猜词和聊天，或查看收发消息日志。

## 代码导航

| 路径 | 职责 |
| --- | --- |
| [`main.cpp`](main.cpp)、[`mainwindow.cpp`](mainwindow.cpp) | 应用入口、主窗口和房间与回合逻辑 |
| [`canvas.cpp`](canvas.cpp) | 笔迹渲染、本地绘图操作和远端笔迹回放 |
| [`connect.cpp`](connect.cpp) | WebSocket 连接、收发和错误处理 |
| [`GameTimer.cpp`](GameTimer.cpp)、[`ChatWindow.cpp`](ChatWindow.cpp) | 回合计时、猜词弹窗和聊天窗口 |
| [`SqliteTools.cpp`](SqliteTools.cpp) | 本地计分与排行榜查询 |
| [`src/Fancy/`](src/Fancy/)、[`src/resource/`](src/resource/) | 界面组件与相关图标、字体资源 |
| [`docs/`](docs/) | 聊天说明、调试记录、历史修复和窗口实现说明 |

## 常见问题

| 现象 | 检查方法 |
| --- | --- |
| CMake 找不到 Qt 或组件 | 在 Qt Creator 中检查 Kit，或检查 `CMAKE_PREFIX_PATH` 是否指向实际 Qt 套件目录，以及该套件是否装有 WebSockets 等组件。 |
| 双击程序提示缺少 DLL 或平台插件 | 从 Qt Creator 或配置了 Qt 和编译器路径的终端启动。需要分发时，使用同一 Qt 套件的 `windeployqt`。 [Qt Windows 部署文档](https://doc.qt.io/qt-6/windows-deployment.html) |
| 一直连接失败 | 确认兼容服务端已启动，并核对地址、端口、WebSocket 路径及 `ws://` 或 `wss://` 协议。远程玩家需要填写各自可访问的地址。 |
| 连上了但房间、绘图或聊天没有同步 | 确认两个客户端加入同一房间，用浏览器调试页观察服务端响应，并在 Qt Creator 的「应用程序输出」查看客户端日志。 |

仓库根目录暂未声明项目整体的许可证。随仓库提供的 `magic_enum` 头文件保留其 MIT 许可声明。
