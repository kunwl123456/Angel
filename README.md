1、项目名字: angel             


2、项目架构简单说明                    
大型世界MMORPG游戏框架，跨平台开发，整体使用共享内存、跨平台协程切换，recast做寻路，G3D做玩家坐标表示和技能范围计算，protobuf做数据传输，tinyxml做起服配置读取，协程rpc请求


3、目录解释                 
1)Excel  保存策划表和打表脚本       
2)Server   服务器代码             
3)Shared   与客户端共享的lua代码或变量文件             
4)Tool     打表工具和一些tolua转C++代码为lua代码的工具(后续逐步上传)                 


4、整体架构图片(VS)
![image](https://github.com/kunwl123456/angel/blob/main/msvc.png?raw=true)

5、博客附加说明（未完待续）                
1）内存池                        
2）协程及汇编切换                 
3）为什么选择单进程？               
4）mysql和redis削峰性能测试       
5）中间件跨服选择                      



# angel
# angel
