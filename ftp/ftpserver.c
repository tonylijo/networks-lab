/* tcpserver.c */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define KEY 13
char **strsplit(char **,char *,char **);
void put(char *,int);
void get(char *,int);

int main()
{
        int sock, connected, bytes_recieved , true = 1,fd;  
        char send_data [1024] , recv_data[1024];      
        struct sockaddr_in server_addr,client_addr;    
        int sin_size;
        
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("Socket");
            exit(1);
        }

        if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&true,sizeof(int)) == -1) {
            perror("Setsockopt");
            exit(1);
        }
        
        server_addr.sin_family = AF_INET;         
        server_addr.sin_port = htons(5000);     
        server_addr.sin_addr.s_addr = INADDR_ANY; 
        bzero(&(server_addr.sin_zero),8); 

        if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))
                                                                       == -1) {
            perror("Unable to bind");
            exit(1);
        }

        if (listen(sock, 5) == -1) {
            perror("Listen");
            exit(1);
        }
		
	printf("TCPServer Waiting for client on port 5000\n");
        fflush(stdout);


        while(1)
        {  

            sin_size = sizeof(struct sockaddr_in);

            connected = accept(sock, (struct sockaddr *)&client_addr,&sin_size);

            printf("I got a connection from (%s , %d)\n",
                   inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

            while (1)
            {
              printf("ftp>");
              fflush(stdout);
              bytes_recieved = recv(connected,recv_data,1024,0);
              recv_data[bytes_recieved] = '\0';
              	printf("recived %s \n" , recv_data);
              	fflush(stdout);
	      if(strcmp(recv_data,"bye")!= 0) {
		if(strncmp(recv_data,"put",3) ==  0) {
			put(recv_data,connected);
		} else if(strncmp(recv_data,"get",3) == 0) {
			get(recv_data,connected);
		}
	      } else {
		printf("closing connetion from (%s,%d)\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));	
		close(connected);
		break;
            }
        }
	}       

      close(sock);
      return 0;
}
void put(char *recv_data,int connected)
{
	int fd,bytes_recieved,read_count;
	char send_data[1024];
	char *str2 = malloc(sizeof(char)*100);
	strcpy(str2,recv_data);
	char **array = malloc(sizeof(char *)*10);
	array = strsplit(&str2," \n",array);
	//printf("filename in server is %s\n",*(array + 1));
	if((fd = open(*(array + 1),O_WRONLY|O_CREAT,0777)) > 0) {
	//	printf("%d\n",fd);
		send(connected,&fd,sizeof(fd),0);
	//	printf("send fd%d\n",fd);
		bytes_recieved = recv(connected,recv_data,1024,0);
		recv_data[bytes_recieved] = 0;
	//	printf("bytes recieved %s\n",recv_data);
		write(fd,recv_data,bytes_recieved);
	//	printf("written bytes recieved %s\n",recv_data);
		close(fd);
	//	printf("closed file \n");
	} else {
		perror("open:");
		printf("could not open the file %s\n",*(array+1));	
		free(str2);
		free(*array);
		return;
	}
	free(str2);
	free(*array);
	fflush(stdout);

}
void get(char *recv_data,int connected)
{
	int fd,bytes_recieved,read_count;
	char send_data[1024];
	char *str2 = malloc(sizeof(char)*100);
	strcpy(str2,recv_data);
	char **array = malloc(sizeof(char *)*10);
	array = strsplit(&str2," \n",array);
	//printf("filename in server is %s\n",*(array + 1));
	if((fd = open(*(array + 1),O_RDONLY)) > 0) {
	//	printf("%d\n",fd);
		send(connected,&fd,sizeof(fd),0);
		bytes_recieved = recv(connected,recv_data,1024,0);
		recv_data[bytes_recieved] = 0;
		if(*((int *) recv_data) > 0) {
			int read_count = read(fd,send_data,1024);
			send(connected,send_data,read_count,0);
		} else {
	//		printf("cound not write to the file\n");
			free(str2);
			free(*array);
			return;
		}
		close(fd);
	} else {
		send(connected,&fd,sizeof(fd),0);
		bytes_recieved = recv(connected,recv_data,1024,0);
		recv_data[bytes_recieved] = 0;
		perror("open:");
		printf("could not open the file %s\n",*(array+1));	
		free(str2);
		free(*array);
		return;
	}
	free(str2);
	free(*array);
	fflush(stdout);
}
