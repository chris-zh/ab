all:qdserver qdclient qdmultiserver
qdserver:server.c
	gcc server.c -o qdserver.exe
qdclient:client.c
	gcc client.c -o qdclient.exe
qdmultiserver:multiplexingServer.c
	gcc multiplexingServer.c -o qdmultiserver.exe
