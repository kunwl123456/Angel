# Angel

> 分布式 MMO 服务端骨架：多进程拆分、内部总线通信、按区服水平扩展。

Angel 是一套面向大型多人在线游戏的 C++ 服务端架构雏形，覆盖客户端接入、逻辑服、场景服、数据缓存与持久化等典型分层。当前仓库以**架构与基础库**为主，多数业务进程仍为占位实现，适合作为 MMO 服务端演进的起点。

## 架构亮点

- **多进程拆分**：Gateway / GameServer / SceneServer / DB 分层，逻辑与 IO 解耦，便于按区服扩容。
- **WBUS 内部总线**：服间走 Channel，客户端经 Gateway 走 Peer，地址编码为 `area.zone.type.index`。
- **全局唯一对象 ID**：64 位 ID 嵌入时间、GS 索引、序号、区服与类型，支持多 GS 水平扩展。
- **Protobuf 协议管线**：CMake 已接入 `protoc` 预处理，协议目录约定已预留（文件待补）。
- **DBCache 分层**：`USE_DBCACHE` 已开启，设计为 GameServer 经 DBManager 访问 DBCache，再落库。
- **NFShmXFrame 风格共享内存**：接入插件生命周期与共享内存模块，旧 `shm.cpp` 已下线，`CShmObject<T>` 走新的 Angel 共享内存后端。
- **LuaBind 脚本模块骨架**：CMake 可探测 `NFShmXFrame/thirdparty/LuaBind`，服务生命周期已预留 LuaLoader、脚本热更与 GM 函数入口。
- **寻路与坐标预留**：集成 RecastNavigation 第三方库，坐标 Agent / 地形 Agent 类骨架已声明。
- **优雅停服**：`GAME_SERVER_STATE_TYPE` 定义多阶段停服状态机（清消息、存角色、踢人等）。
- **配置热更**：`LoadCfgByConfigParam` 支持多类配置项与可选 reload 线程。

## 进程与服务角色

## 与 MMORPG 架构文章对照

本文档参考了 [游戏思考13：大型 MMORPG 游戏服务器分类及作用](https://blog.csdn.net/weixin_43679037/article/details/124957453) 中对大型 MMORPG 后端的分层方式，并按 Angel 当前代码做了映射：

- **Gateway / LoginGate**：对应 `svrGW` 与 `gateway_server`，负责客户端连接、会话、消息转发，是客户端进入内网服务的唯一入口。
- **GameCenter / 大厅服**：对应 `svrWS` 与 `world_server`。按 Angel 风格命名为 WorldServer，后续可承载选角、非场景业务、GS 路由与世界级协调。
- **GameServer**：对应 `svrGS` 与 `game_server`，负责角色玩法逻辑、对象 ID、与 SceneServer / DB 层协作。
- **SceneServer**：对应 `svrSS` 与 `scene_server`，负责地图实例、实体位置、移动片段、AOI 与场景内广播。
- **DBManager / DBCache**：对应 `dbmanager_server` 与 `dbcache_server`，保持 DB 代理 + 缓存层分离。
- **状态同步取向**：MMORPG 优先采用服务器权威的状态同步；移动可用 `MovementFragment` 这类移动片段降低每帧同步带宽。

| 缩写 | 枚举 | 设计职责 | 可执行进程 | 实现状态 |
|------|------|----------|------------|----------|
| GW | `svrGW` | 客户端接入、会话、转发 C2S/S2C | `gateway_server` | 框架可运行，业务待实现 |
| GS | `svrGS` | 核心玩法逻辑、对象管理、协调各服 | `game_server` | 框架可运行，已接 `CGameWorld` 主循环 |
| WS | `svrWS` | 世界/大区协调（GameCenter / 大厅服） | `world_server` | 框架可运行，业务待实现 |
| SS | `svrSS` | 场景实例、副本、AOI 等 | `scene_server` | 框架可运行，AOI/移动同步待实现 |
| PS | `svrPS` | 平台/支付类服务 | `platform_server` | 框架可运行，业务待实现 |
| TS | `svrTS` | 组队/匹配类服务 | `team_server` | 框架可运行，业务待实现 |
| CS | `svrCS` | 跨服玩法 | `cross_server` | 框架可运行，业务待实现 |
| MCS | `svrMCS` | 主控/协调类服务 | `master_control_server` | 框架可运行，业务待实现 |
| FS | `svrFS` | 好友/社交 | `friend_server` | 框架可运行，业务待实现 |
| Guild | `svrGuild` | 公会系统 | `guild_server` | 框架可运行，业务待实现 |
| Global | `svrGlobal` | 全服唯一数据 | `global_server` | 框架可运行，业务待实现 |
| — | — | DB 路由与持久化编排 | `dbmanager_server` | 框架可运行，持久化待实现 |
| — | — | 角色/数据缓存层 | `dbcache_server` | 框架可运行，缓存逻辑待实现 |

> 仅当 CMake **构建目录名为 `angel`** 时，才会编译上表中的业务进程（见 [Server/build/CMakeLists.txt](Server/build/CMakeLists.txt)）。

## 架构拓扑

```mermaid
flowchart LR
  Client[Client]
  GW[gateway_server]
  WS[world_server]
  GS[game_server]
  SS[scene_server]
  Team[team_server]
  Guild[guild_server]
  Friend[friend_server]
  Cross[cross_server]
  Platform[platform_server]
  MCS[master_control_server]
  DBM[dbmanager_server]
  DBC[dbcache_server]
  Global[global_server]

  Client -->|Peer_WBUS| GW
  GW -->|Channel| WS
  WS -->|Route| GS
  GS -->|Channel| SS
  GS --> Team
  GS --> Guild
  GS --> Friend
  GS -.-> Cross
  WS --> Platform
  MCS -.-> WS
  MCS -.-> GS
  GS --> DBM
  DBM --> DBC
  GS -.-> Global
```

**库与进程依赖（静态库 → 可执行文件）：**

```mermaid
flowchart TB
  subgraph libs [静态库]
    base_lib[base_lib]
    core_lib[core_lib]
    game_data[game_data]
    game_world[game_world]
    server_base[server_base]
  end
  subgraph exes [可执行进程_angel构建]
    gs[game_server]
    gw[gateway_server]
    ws[world_server]
    ss[scene_server]
    ps[platform_server]
    ts[team_server]
    cs[cross_server]
    mcs[master_control_server]
    fs[friend_server]
    guild[guild_server]
    global[global_server]
    dbm[dbmanager_server]
    dbc[dbcache_server]
  end
  base_lib --> core_lib
  base_lib --> game_data
  base_lib --> game_world
  base_lib --> server_base
  base_lib --> nfshm[NFShm-style shm plugin]
  server_base --> lua[LuaBind script module]
  core_lib --> server_base
  game_world --> gs
  server_base --> gs
  server_base --> gw
  server_base --> ws
  server_base --> ss
  server_base --> ps
  server_base --> ts
  server_base --> cs
  server_base --> mcs
  server_base --> fs
  server_base --> guild
  server_base --> global
  server_base --> dbm
  server_base --> dbc
```

## 消息与数据流

### 典型玩家路径（设计目标）

1. **登录**：Client → Gateway（Peer）鉴权与会话建立 → Channel 转发至 GameServer。
2. **进场景**：GameServer 分配/选择 SceneServer → 创建或挂载场景对象，下发可见集（AOI）。
3. **切场景**：GameServer 协调源/目标 SceneServer，迁移对象状态并更新 Gateway 路由。
4. **存档**：GameServer 经 DBManager 写入 DBCache，再异步或同步落持久化存储。

```mermaid
sequenceDiagram
  participant C as Client
  participant GW as Gateway
  participant GS as GameServer
  participant SS as SceneServer
  participant DB as DBManager_DBCache

  C->>GW: 连接与登录
  GW->>GS: Channel转发
  GS->>SS: 进入场景
  SS-->>GW: 场景内推送
  GW-->>C: Peer下发
  GS->>DB: 角色存档
```

### WBUS 通信模型

- **地址**：`wbus_addr(area, zone, type, index)`，字符串形式如 `area.zone.type.index`（见 `server_def.h` / `wbus_def.h`）。
- **Channel**：服务器之间的可靠消息通道（`channel_send` / `channel_sendv`）。
- **Peer**：客户端与 Gateway 之间的连接（`peer_send`，代码注释 `peer -- gateway`）。

### 配置热更

进程可通过 `LoadCfgByConfigParam` 加载多类配置，主要类型包括：

| 类型 | 说明 |
|------|------|
| `CONFIG_TYPE_MAIN` | 进程主配置 |
| `CONFIG_TYPE_WHITELIST` | GameServer 白名单 |
| `CONFIG_TYPE_TESTACNT` | 测试账号 |
| `CONFIG_TYPE_MSGFREQLIST` | 消息频率限制 |
| `CONFIG_TYPE_SSMSGFREQLIST` | SceneServer 消息频率限制 |
| `CONFIG_TYPE_DBOPFREQLIST` | DB 操作频率限制 |
| `CONFIG_TYPE_GAMETOOS` | 运维工具配置 |
| `CONFIG_TYPE_CLIENTRESFOLDERS` | 需下发给客户端的资源目录 |
| `CONFIG_TYPE_LOCAL_RELAY_TESTACNT` | 本地服白名单 |

`CGameServer::CreateReloadThread` 预留了配置热更线程入口（实现待补）。

### 对象 ID 与类型

对象 ID 为 64 位紧凑编码（`OBJECT_ID`）：时间戳 + GS 索引 + 序号 + 区 + 类型。  
`CGameWorld::generate_obj_id` 已在 GameServer 侧实现生成逻辑。

支持的对象类型（`OBJECT_TYPE`）：Role、Npc、Doodad、Missile、Build、SceneItem、Volume。

## 目录结构

```
Angel/
├── README.md           # 本文件：架构总览
├── Server/             # C++ 服务端主体
│   ├── build/          # CMake 工程（构建目录须命名为 angel）
│   ├── inc/            # 头文件（按模块分目录）
│   ├── src/            # 源码
│   │   ├── base_lib/       # WBUS、网络、队列、定时器、NFShm 风格共享内存
│   │   ├── core_lib/
│   │   ├── game_data/
│   │   ├── game_world/     # 游戏世界、坐标、对象 ID
│   │   ├── server_base/    # 应用框架、日志、配置
│   │   ├── gateway_server/
│   │   ├── world_server/
│   │   ├── game_server/
│   │   ├── scene_server/
│   │   ├── platform_server/
│   │   ├── team_server/
│   │   ├── cross_server/
│   │   ├── master_control_server/
│   │   ├── friend_server/
│   │   ├── guild_server/
│   │   ├── global_server/
│   │   ├── dbmanager_server/
│   │   ├── dbcache_server/
│   │   └── misc/
│   ├── lib/            # 编译产物（静态库）
│   ├── bin/            # 可执行文件输出目录
│   └── 3rd/            # 第三方：protobuf、RecastNavigation、zlib 等
├── Shared/             # 跨端共享协议（proto，目录已预留，文件待补）
├── Excel/              # 策划表工具链（规划中）
└── Tool/               # 辅助工具（规划中）
```

## NFShmXFrame 接入说明

本轮迁移将 Angel 原先的轻量共享内存骨架替换为 **NFShmXFrame 风格的插件式共享内存后端**：

- 新增 `base_lib/nf_shm/angel_plugin_runtime.h`：提供 `Install / Awake / Init / Tick / Shut / Finalize` 插件生命周期。
- 新增 `base_lib/nf_shm/angel_shm_module.h`：集中管理共享内存段、启动模式与对象创建模式。
- 新增 `base_lib/nf_shm/angel_shm_registry.h`：登记共享内存对象类型、稳定类型 ID、容量和创建/恢复/销毁回调。
- `base_lib/shm_object.h` 保留为兼容入口，但内部已转向新的 `CAngelShmObject<T>`。
- 原 `base_lib/shm.h` 与 `src/base_lib/vm/shm.cpp` 已移除，避免新旧共享内存实现并存。

当前迁移重点是把 Angel 接到 NFShmXFrame 的**对象生命周期与插件模型**上；后续业务对象可逐步迁移为更完整的 `NFObject / NFObjectTemplate / ObjSeg` 风格。
正式业务对象需要显式分配稳定 `typeId`，不能依赖编译器生成名称；详见 [docs/shm-plugin.md](docs/shm-plugin.md)。

## LuaBind 与脚本模块

Angel 现在预留了 LuaBind 脚本模块入口：

- `server_base/lua/angel_lua_loader.h`：封装脚本根目录、重载脚本、GM 函数调用等基础接口。
- `CSsGameApp` 生命周期中会初始化 `CAngelLuaScriptModule`，并在主循环中 Tick。
- CMake 会探测 `../NFShmXFrame/thirdparty/LuaBind`，存在时定义 `ANGEL_WITH_NFSHMXFRAME_LUABIND` 并构建 `angel_luabind` 静态库。
- LuaLoader 已支持加载脚本文件、执行 Lua 字符串、重载已加载脚本、调用全局 GM 函数。

这一步先保持 Angel 原配置加载方式不被破坏，Lua 配置、热更和 GM 逻辑会在后续业务层逐步接入。

## 构建

**前置条件**

- Windows：Visual Studio（CMake 生成 MSVC 工程）
- Linux：GCC 4.8+，C++11
- 构建目录名必须为 **`angel`**，否则不会编译业务进程
- 若要启用 LuaBind，需同级目录存在 `NFShmXFrame/thirdparty/LuaBind`

**跨平台约束**

- 新增代码需要用 `_WIN32/_WIN64` 与 Linux 分支隔离平台头文件，避免在 Windows 下包含 `sys/*` / `unistd.h` / `strings.h`。
- WBUS 时间函数通过 Angel 封装处理，Linux 走 `gettimeofday`，Windows 走 `GetSystemTimeAsFileTime`。
- 共享内存模块 Linux 使用 `shm_open/mmap`，Windows 使用 `CreateFileMapping/MapViewOfFile`。
- 修改跨平台基础头后，至少跑一次 Linux Docker 构建；Windows 侧建议在 VS Developer PowerShell 下使用 README 的 MSVC 构建命令验证。

**Docker/Linux 验证环境**

本轮 NFShmXFrame 迁移已用 `gcc:11` 容器完成 Debug 构建验证。验证命令如下：

```powershell
docker run --rm -v "d:/3rd:/work" gcc:11 bash -lc "set -e; \
  apt-get update >/tmp/apt-update.log; \
  apt-get install -y cmake protobuf-compiler libprotobuf-dev libevent-dev libexpat1-dev >/tmp/apt-install.log; \
  rm -rf /tmp/angel && mkdir -p /tmp/angel && cd /tmp/angel; \
  cmake /work/Angel/Server/build -DCMAKE_BUILD_TYPE=Debug; \
  cmake --build . -j2"
```

验证结果：`angel_luabind`、`base_lib`、`core_lib`、`game_data`、`game_world`、`server_base` 以及所有 Angel 业务进程（Gateway / Game / Scene / DB / Team / Guild / Global 等）均构建完成。

**Windows（示例）**

```powershell
cd Server
mkdir build\angel
cd build\angel
cmake ..\..\build -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

**Linux（示例）**

```bash
cd Server
mkdir -p build/angel && cd build/angel
cmake ../../build -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

**产物路径**

- 可执行文件：`Server/bin/<进程名>/`（Debug 带 `_d` 后缀）
- 静态库：`Server/lib/`

**协议代码生成**

CMake 在配置阶段会调用 `protoc` 处理以下路径（若目录为空或缺少 `.proto` 会导致配置失败）：

- `Shared/proto`
- `Server/src/game_data/proto`
- `Server/src/game_world/proto`

## 配置与协议（规划）

当前仓库**尚未包含**具体配置文件样例；以下为代码中已约定的扩展点，实施时需自行补齐：

- 各进程主配置：对应 `CONFIG_TYPE_MAIN`，由 `ProcConfNode` 描述路径与 metalib 绑定。
- 协议定义：建议客户端与服务器共用 `Shared/proto`，服内模块协议放在 `game_data/proto`、`game_world/proto`。
- 日志、区服 ID、WBUS 路由表：预期在进程主配置与 `CSsGameApp` 初始化流程中加载（`ss_game_app` 框架已声明接口）。

## 当前进度与路线图

**已完成 / 有实质代码**

- `base_lib`：WBUS 地址与网络层、环形缓冲、异步消息队列、定时器、NFShm 风格共享内存插件
- `server_base`：统一应用生命周期已接入共享内存插件与 LuaScript 模块骨架
- 构建修复：Linux/GCC 下补齐基础类型别名、跨平台头文件、libevent/protobuf/tinyxml 链接和 LuaBind 构建入口
- `game_world`：游戏世界单例、`generate_obj_id`，新增 `CAoiGrid` 九宫格 AOI 骨架
- `game_data`：新增 `CRoleData` / `CRoleDataCache`，预留角色加载、脏数据标记、停服 flush 的 DBCache 闭环
- `game_server`：`CGameServer` 单例、角色缓存入口、`CGameWorld` 主循环与 `CFramServer` 停服钩子
- `define`：服务器类型、停服状态机、配置类型、行为树节点枚举
- CMake 工程与 `protoc` 预处理管线
- RecastNavigation 第三方源码 vendoring

**进行中 / 骨架**

- `gateway_server`、`world_server`、`game_server`、`scene_server`：已接入统一 `CSsGameServerApp` 主循环，业务逻辑待实现
- `platform_server`、`team_server`、`cross_server`、`master_control_server`、`friend_server`、`guild_server`、`global_server`：已建立 Angel 风格进程骨架
- `dbmanager_server`、`dbcache_server`：已接入统一 `CSsGameServerApp` 主循环；DBCache 已接角色缓存骨架，持久化落库仍待实现
- `misc`：`main` 仍为占位
- `coordinate_*`、`WkCoordAgent`：类声明，无业务逻辑
- `CSsGameApp`：已补最小 runtime 桥接，完整 TAPP 行为仍待扩展
- `Excel/`、`Tool/`：空目录

**待做**

- 补齐完整 `.proto` 与 C/S 消息分发
- 实现 Gateway 会话与 GS 路由
- SceneServer AOI 已有网格骨架；后续补真实移动消息、可见集变化广播与场景实例
- DBManager / DBCache 网络消息、缓存淘汰、批量落库与异常恢复
- 配置样例、起服脚本与本地联调环境
- World / Platform / Team / Cross / Guild / Global 等进程的业务逻辑
- 共享内存业务对象类型 ID、版本校验、恢复策略与数据回滚工具
- Lua 配置、Lua 热更、GM 权限控制与脚本沙箱

## MMO 后续迭代需求

Angel 要演进为可用 MMO 服务端，建议按以下顺序推进：

1. **账号与网关会话**：登录鉴权、断线重连、限流、封禁、Gateway 到 GameServer 的路由表。
2. **角色与 DBCache 闭环**：角色创建/加载/保存已有内存缓存骨架；后续补网络消息、缓存淘汰、DBManager 到持久化存储。
3. **场景与 AOI**：九宫格 AOI 骨架已接入；后续补地图实例、移动广播与兴趣管理。
4. **核心玩法对象化**：角色、NPC、掉落、技能、Buff 等统一接入共享内存对象生命周期。
5. **Lua 热更体系**：配置 Lua 化、GM 命令、玩法脚本热更、脚本权限与回滚。
6. **跨服与全局服务**：公会、好友、匹配、跨服战、排行榜、全局唯一数据。
7. **运维与可观测性**：起停服编排、指标、日志聚合、共享内存检查、在线热修复审计。

## 常见问题

- **CMake 配置失败，提示 proto 相关错误**  
  需先在 `Shared/proto`、`Server/src/game_data/proto`、`Server/src/game_world/proto` 中放置至少一个有效的 `.proto` 文件，或暂时调整 `Server/build/CMakeLists.txt` 中的 `preprocess_proto` 调用（开发阶段）。

- **编译成功但没有 `game_server` 等可执行文件**  
  确认构建目录名为 `angel`（`FOLDER_NAME STREQUAL "angel"` 才会 `add_subdirectory` 业务进程）。

- **进程启动后没有业务响应**  
  当前进程已接入 `CSsGameApp` 主循环，但登录、场景、DB 等业务仍是骨架，需要继续补消息分发和数据流程。

- **对象 ID 生成失败**  
  需确认 `CGameWorld::init` 已在 `game_server` 启动时执行；对象类型必须位于 `otRole` 到 `otTotal` 范围内。
