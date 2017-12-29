all:qdserver qdclient
qdserver:server.c
	gcc server.c -o qdserver
qdclient:client.c
	gcc client.c -o qdclient
