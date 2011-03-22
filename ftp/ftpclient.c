/* tcpclient.c */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#define _DEBUG
char **strsplit(char **,char *,char **);
void put(char *,int);
void get(char *,int);
void print_help(void);
int main()
{

        int sock, bytes_recieved,fd;  
        char send_data[1024],recv_data[1024];
        struct hostent *host;
        struct sockaddr_in server_addr;  
        host = gethostbyname("127.0.0.1");
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("Socket");
            exit(1);
        }
        server_addr.sin_family = AF_INET;     
        server_addr.sin_port = htons(5000);   
        server_addr.sin_addr = *((struct in_addr *)host->h_addr);
        bzero(&(server_addr.sin_zero),8); 

        if (connect(sock, (struct sockaddr *)&server_addr,
                    sizeof(struct sockaddr)) == -1) 
        {
            perror("Connect");
            exit(1);
        }

        while(1)
        {
           printf("ftp>");
	   fflush(stdout);
           gets(send_data);
	   if((strcmp(send_data,"bye")==0) || (strcmp(send_data,"quit") ==0)) {
           	send(sock,send_data,strlen(send_data), 0);
		break;
	   } else if(strncmp(send_data,"put ",4) == 0) {
		put(send_data,sock);
	   } else if(strncmp(send_data,"get ",4) == 0) {
		get(send_data,sock);
	   } else if(strcmp(send_data,"help") ==0) {
		print_help();
	   } 
        }   
	return 0;
}
void print_help(void)
{
	printf("commands:\nput \t get \t help \t bye \t quit \n");
}
void put(char *send_data,int sock) 
{
	int fd,bytes_recieved,read_count;
	char recv_data[1024];
	char *str2 = malloc(sizeof(char)*100);
	strcpy(str2,send_data);
	char **array = malloc(sizeof(char *)*10);
	array = strsplit(&str2," \n",array);
	#ifdef _DEBUG
	printf("filename in get %s\n",*(array+1));
	#endif
	if((fd = open(*(array + 1) , O_RDONLY)) < 0) {
		perror("open");
		free(str2);
		free(*array);
		return;
	}
	send(sock,send_data,strlen(send_data),0);
	#ifdef _DEBUG
	printf("send the SEND_DATA:%s",send_data);
	#endif
	bytes_recieved = recv(sock,recv_data,1024,0);
	recv_data[bytes_recieved] = 0;
	#ifdef _DEBUG
	printf("bytes recieved %s\n",recv_data);
	#endif
	if(*((int *) recv_data) > 0) {
		int read_count = read(fd,send_data,1024);
		send(sock,send_data,read_count,0);
		#ifdef _DEBUG
		printf("send buffer %s\n",send_data);
		#endif
	} else {
		printf("cound not write to the file\n");
		free(str2);
		free(*array);
		return;
	}
	#ifdef _DEBUG
	printf("bytes recieved = %d\n",*((int *) recv_data));
	#endif
	free(str2);
	free(*array);
	close(fd);
}	
void get(char *send_data,int sock) 
{
	int fd,bytes_recieved,read_count;
	char recv_data[1024];
	char *str2 = malloc(sizeof(char)*100);
	strcpy(str2,send_data);
	char **array = malloc(sizeof(char *)*10);
	array = strsplit(&str2," \n",array);
	#ifdef _DEBUG
	printf("filename in get %s\n",*(array+1));
	#endif
	if((fd = open(*(array + 1) , O_WRONLY|O_CREAT,0777)) < 0) {
		perror("open");
		free(str2);
		free(*array);
		return;
	}
	send(sock,send_data,strlen(send_data),0);
	bytes_recieved = recv(sock,recv_data,1024,0);
	recv_data[bytes_recieved] = 0;
	send(sock,&fd,sizeof(fd),0);
	#ifdef _DEBUG
	printf("%d\n",*((int *) recv_data));
	#endif
	if(*((int *) recv_data) > 0) {
		bytes_recieved = recv(sock,recv_data,1024,0);
		recv_data[bytes_recieved] = 0;
		write(fd,recv_data,bytes_recieved);
		close(fd);
	} else {
		printf("problem in server\n");
		free(str2);
		free(*array);
		close(fd);
		return;
	}
	free(str2);
	free(*array);
}
	
