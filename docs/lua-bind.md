# Angel LuaBind 接入设计

## 定位

NFShmXFrame 的 Lua 体系由 `thirdparty/LuaBind`、`NFILuaLoader`、`NFILuaScriptModule`、`NFCConfigModule` 和 `NFProtobufCommon::LuaToProtoMessage` 组成。本次 Angel 先接入 LuaBind 构建与脚本模块入口，保持现有配置加载可用，不一次性重写全部业务。

## 当前落点

| 文件 | 作用 |
|------|------|
| `Server/inc/server_base/lua/angel_lua_loader.h` | Angel 侧 LuaLoader 与脚本模块接口 |
| `Server/src/server_base/lua/angel_lua_loader.cpp` | 脚本根目录、文件加载、字符串执行、重载、GM 调用 |
| `Server/build/CMakeLists.txt` | 探测 `NFShmXFrame/thirdparty/LuaBind` 并构建 `angel_luabind` |
| `CSsGameApp` | 初始化脚本模块并在 Tick 中驱动 |

## 设计原则

- **不破坏现有配置**：`CGameCfg::Load` 仍是主路径，Lua 配置作为扩展。
- **先接入口，再迁业务**：Loader 已支持 `LoadScriptFile`、`ExecuteString`、`ReloadLuaFiles`、`RunGmFunction`，后续再接具体玩法脚本。
- **保持 Angel 风格**：业务层看到的是 `CAngelLuaScriptModule`，不是直接散落 Lua C API。

## 后续工作

1. 将服务器主配置、插件配置、日志配置逐步支持 Lua 表描述。
2. 增加 `LuaToProto` 映射，让策划配置可直接生成 Protobuf 数据。
3. 将热更接口接到控制台命令：单文件热更、全量热更、GM 函数执行。
4. 增加脚本安全：白名单 API、执行超时、错误隔离、热更回滚。
5. 为 GameServer/SceneServer 提供玩法脚本注册点。
