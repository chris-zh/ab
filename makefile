all:qdserver qdclient
qdserver:server.c
	gcc server.c -o qdserver.exe
qdclient:client.c
	gcc client.c -o qdclient.exe
