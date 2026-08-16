.PHONY:all
all:Client Server #不写依赖方法 只写依赖关系 表明我需要的可执行文件
#直接执行make进行编译时 make会从上往下 依次进行依赖关系信息的检索
Client:Client.cc
	g++ -o $@ $^ -std=c++11

Server:Server.cc
	g++ -o $@ $^ -std=c++11


.PHONY:clean
clean:
	rm -f Client Server