CC := gcc 
CFLAGS :=
#DEBUG1 := -D _DEBUG
#DEBUGFLAGS := -g $(DEBUG1)
server:	ftpserver.o string.o client
	$(CC) $(CFLAGS) $(DEBUGFLAGS) ftpserver.o string.o -o server
client:	ftpclient.o string.o
	$(CC) $(CFLAGS) $(DEBUGFLAGS) ftpclient.o string.o -o client
ftpserver.o: ftpserver.c
	$(CC) -c $(DEBUGFLAGS) ftpserver.c
string.o: string.c
	$(CC) -c $(DEBUGFLAGS) string.c
clean:
	rm *.o
copy:
	cp server server_dir
	cp client client_dir
