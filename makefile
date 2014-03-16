linux_server_main :linux_server_main.o fileData.o
	g++ linux_server_main.o fileData.o -o linux_server_main -lboost_system
linux_server_main.o :linux_server_main.cpp
	g++ -c linux_server_main.cpp
fileData.o :fileData.cpp fileData.h
	g++ -c fileData.cpp
clean :
	rm *.o linux_server_main
