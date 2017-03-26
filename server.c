#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <stdlib.h>
#include <strings.h>
#include <sys/wait.h>	/* for the waitpid() system call */
#include <signal.h>	/* signal name macros, and the kill() prototype */
#include <string.h>
#include <unistd.h>

void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

void dostuff(int); /* function prototype */
void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portno, pid;
	socklen_t clientlen;
	struct sockaddr_in serv_addr, client_addr;
	struct sigaction sa;

	if (argc < 2){
		fprintf(stderr, "ERROR, no port number provied\n");
		exit(1);
	}
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
		error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = PF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if ( bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) <0)
		error("ERROR on binding");
	listen(sockfd, 5);

	clientlen=sizeof(client_addr);

 /****** Kill Zombie Processes ******/
     sa.sa_handler = sigchld_handler; // reap all dead processes
     sigemptyset(&sa.sa_mask);
     sa.sa_flags = SA_RESTART;
     if (sigaction(SIGCHLD, &sa, NULL) == -1) {
         perror("sigaction");
         exit(1);
     }
     /*********************************/

while(1){			
	newsockfd = accept(sockfd, (struct sockaddr *) &client_addr, &clientlen);
	if (newsockfd < 0)
		error("ERROR on accept");

	pid = fork();
	if (pid<0)
		error("ERROR on fork");

	if (pid == 0){
		close(sockfd);
		dostuff(newsockfd);
		exit(0);
	}
	else
		close(newsockfd);
	}
	return 0;
}	

void dostuff(int sock){
	
//PartA
	int n;
	char buffer[512];
	
	bzero(buffer, 512);
	n = read(sock, buffer, 256);
	if (n<0) 
		error("ERROR reading from socket");
	printf("Message from client: %s\n", buffer);

//PartB
	int i = 0;
	char request[50];
	int j = 0;
	for(i = 0; i < strlen(buffer); i++){
		if(i+2 < strlen(buffer)){
			if(buffer[i] == 'G' && buffer[i+1] == 'E' && buffer[i+2] == 'T')
				break;
		}
	}
	i = i+3;
	if(buffer[i] == ' '){
		i+=2;
		while(buffer[i] != ' '){
			request[j++] = buffer[i++];
		}
		request[j] = '\0';
	}
	int reqType;
	if(strlen(request) == 0 || strlen(request) == 1)
		reqType = 0;
	else{
		int k = 0;
		while(request[k] != '.'){
			k++;
		}
		char fileType[50];
		int l = 0;
		for(k = k+1; k < strlen(request); k++){
			fileType[l++] = request[k];
		}
		fileType[l] = '\0';
		if(!strcmp(fileType, "html") || !strcmp(fileType, "htm"))
			reqType = 1;
		else if(!strcmp(fileType, "jpeg") || !strcmp(fileType, "jpg"))
			reqType = 2;
		else if(!strcmp(fileType, "gif"))
			reqType = 3;
	}

	char *file_contents;
	size_t size;
	if(access(request, F_OK) != -1){
		FILE* ifp = fopen(request, "r");
		long input_file_size;
		fseek(ifp, 0, SEEK_END);
		input_file_size = ftell(ifp);
		rewind(ifp);
		file_contents = (char*)malloc((input_file_size + 1) * (sizeof(char)));
		size = fread(file_contents, sizeof(char), input_file_size, ifp);
		fclose(ifp);
	}
	else{
		reqType = 0;
	}

	char *str;
	str = malloc(100 * sizeof(char));
	if(reqType == 0){
		strcpy(str, "HTTP/1.1 404 NOT_FOUND\r\n");
		//write(sock, str, strlen(str));

	}
	else {
		strcpy(str, "HTTP/1.1 200 OK\r\n");
		printf("%s", str);
		//write(sock, str, strlen(str));
		if(reqType == 1){
			strcat(str, "Content-Type: text/html\r\n\r\n");
			//write(sock, str, strlen(str));
		}else{
			strcat(str, "Content-Type: image/jpeg\r\n\r\n");
			//write(sock, str, strlen(str));
		}
		printf("%s", str);
		//strcat(str, file_contents);
	}
	write(sock, str, strlen(str));
	write(sock, file_contents, size);
}


