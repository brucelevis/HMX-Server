HMX-Server is a distributed service framework!

版权所有代码作者qq296464231所有，代码可用于复制，修改，商业目的等，但不得去掉版权所有标识，或在文件头加于注明即可 

下面两个库也是需要x64
boost 1.60-x64 http://pan.baidu.com/s/1pL6DheN
mysql 5.56-x64 http://pan.baidu.com/s/1nu5KZsH


安装步聚：
从Github下载后，将项目放在E:/NEW_SERVER/目录下（以后优化会取消这个限制，原因如下说明）

本项目文件需要绝对路径或修改默认路径
CommonDefine.h
#define CFG_DIR		"../../Config/"
#define RES_DIR		"E:/NEW_SERVER/Resource/"
#define ZONE_DIR	"E:/NEW_SERVER/Resource/Zone/"
#define SCENE_DIR	"E:/NEW_SERVER/Resource/Scene/"

目录结构：
E:/NEW_SERVER/BIN/
E:/NEW_SERVER/BIN/DEBUG
E:/NEW_SERVER/Resource
E:/NEW_SERVER/Resource/Scene
E:/NEW_SERVER/Resource/Zone
E:/NEW_SERVER/Source
E:/NEW_SERVER/Source/dependence/boost  下载boost解压覆盖这个目录
E:/NEW_SERVER/Source/dependence/mysql  下载mysql解压覆盖这个目录
E:/NEW_SERVER/Source/doc			   项目的一些临时设置思想文档
E:/NEW_SERVER/Source/sql			   导入到数据库
E:/NEW_SERVER/Source/servers		   进程服务器代码

从百度云盘下载我编译好的Boost库与Mysql库，这里需要注意的是，本项目是x64的，所以请在win7-64以上机器测试，mysql要安装5.6-x64以上
导入Source/sql/hmx_data.sql 文件

生成项目文件.sln
1、你可以通过项目结构创建创建项目
2、也可以通过Cmake来构建项目（Cmake中是用1.60 -x64 vs2015 -x64版本设定，如果你们要特定的boost版本与VS版本，我也要可以给你编译和设定，哈哈，要收点手工费哦^_^!）
3、HMX相关的帮助请浏览器官网http://www.hmx-server.org

生成好后，在Bin/Debug/目录下启动，双击RunServer.bat启动它们五个进程，这时要检查各个进程之前的报错，如打印中有[ERROR]:xxxx
再启动测试Client.exe 
在上面测试的过程中会偶尔弹出assert提示，进程会挂掉，测试阶段这样方便找到问题或者知道哪里可能需要优化，
你可以将ASSERT这个宏定义值 assert 改为 printf("assert\n");就不会影响你开发与测试了，后面我继续完善，这些ASSERT会慢慢去掉

cmake 设置为x64 vs
cmake 3.4 以上

vs编译也需要设置为x64
vs2015 



