#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#include<ctype.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<errno.h>
#include<strings.h>
#include<signal.h>
#include<sys/ioctl.h>

//define
#define START "Welcome : Enter the commands to your liking\n"
#define DIVIDE "Error : division by zero\n"
#define MESSAGE "Invalid command, type help\n"
///
void preForkMethod();

char* methodArithmetic(char * token, char * buff);

void printIpAndPort(int portNumber);

int checkDigit(char * token);

pid_t forkChild(int listenfd);

int noOfProcesses;

int checkDigit(char * token);
///

int main()
{

	int sock, length;
	struct sockaddr_in server;
	
	/*Create Sockets*/
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if( sock < 0)
			{
				perror("error: making socket");
				exit(1);
			}
		
	/*server wildcard entries*/	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = 0;
	
	if(bind(sock,(struct sockaddr *) &server, sizeof(server)) < 0)
		{
			perror("error");
			exit(1);
		}
	
	length = sizeof(server);
	
	if(getsockname(sock,(struct sockaddr *) &server,(socklen_t *) &length) < 0)
		{
			perror("error");
			exit(1);
		} 
		
	printIpAndPort(server.sin_port);
	
	listen(sock, 5);
	
	forkChild(sock);
	
	int pid = wait(NULL);
	
	printf("pid is  %d exited\n",pid);

}

void printIpAndPort(int portNumber)
{
	int port = ntohs(portNumber);
	
	char tempBuff[50];
	
	int count = sprintf(tempBuff,"Port number = %d\n", port);
	
	write(STDOUT_FILENO,tempBuff,count);

}

//Arthimetic operations 
char* methodArithmetic(char * token, char * buff)
{
	
	char * identifier = token;
	
	int sum = 0;
	
	int firstNumber = 0;
	
	int divideZero = 0;
	
	int check = 0;
	
	int arguments = 0;
	
	if((token = strtok(NULL," \n")) == NULL)
		{
			strcpy(buff,MESSAGE);
			
			return buff;
		}
	
	
	
	while(token != NULL)
	{
		check = checkDigit(token);
					if(check == 0)
						break;
			
						
		if(strcasecmp("add",identifier) == 0)
			{
				sum += atoi(token);
			}
		else if(strcasecmp("sub",identifier) == 0)
			{	
			
				if(firstNumber == 0)
					{	
						sum = atoi(token);
						
						firstNumber = 1;
					}
				else sum -= atoi(token);
			}
		else if(strcasecmp("mult",identifier) == 0)
			{
				if(firstNumber == 0)
					{	
						sum = atoi(token);
						
						firstNumber = 1;
					}
				else sum *= atoi(token);
			}
		else if(strcasecmp("div",identifier) == 0)
			{
				if(firstNumber == 0)
					{	
						sum = atoi(token);
						
						firstNumber = 1;
					}
				else 
					{ 
						if( atoi(token) == 0)
								{	
									divideZero = 1;
					
									break;
								}
						sum = sum / atoi(token);
					}	
						
			}
	
			arguments++;
	
			token = strtok(NULL," \n");
		}
		
	
	if(check == 0)
		strcpy(buff,"Invalid arguments\n");
	
	else if(divideZero == 1)
				strcpy(buff,DIVIDE);
				
			
	else if ( arguments == 1)
			strcpy(buff,"Too few arguments! Atleast two required\n");
			
	else
		 	{	int count = sprintf(buff,"%d",sum);

		 		buff[count] = '\n';

		 		count++;

		 		buff[count] = '\0';
		 	}
		
	return buff; 	

}	

int checkDigit(char * token)
{
	int i = 0;
	
	char buff[20];
	
	strcpy(buff, token);
	
	int c = 0;
	
	while(i < strlen(token))
	{
		 c  = isdigit(buff[i]);
		 
			if(c == 0)
				break;
		i++;
	}

	if(c  == 0)
		return 0;

	else return 1;
}

//fork()
pid_t forkChild(int listenfd)
{
	for(int i = 0; i<5; i++)
	{
		int pid = fork();
	
		if(pid == 0)
		{	
			
			int writeCountSocket = 0;
			int readCountSocket = 1;
			
			int msgsock = accept(listenfd, 0 ,0);
				
			do{
						char readBuffSocket[100];
						
						bzero(readBuffSocket,100);
						
						int status = 1;
						
						int readCountSocket = 0;
						
						char readBuff;
						
						while(status != 0 )
							{															
								int readFromSocket = read(msgsock,&readBuff,1);
								
								if(readFromSocket == 0)
									break;
							
								readBuffSocket[readCountSocket] = readBuff;
								
								readCountSocket++;
							
								ioctl(msgsock,FIONREAD, &status);
							}
						
						char * arthimeticBuff = (char *)malloc(50*sizeof(char));
						
						int argumentRunArray = 0;
						
						char * resultBuff;
						
						int ignore = 0;
						
						if(readCountSocket == 0)
						 		{
						 			write(STDOUT_FILENO, "Connection ended\n",strlen("Connection ended\n"));
						 			exit(EXIT_SUCCESS);
								}
					
						char * token;
						
						if(readCountSocket - 1 == 0)
							token = "a"; //garbage
							
						else token = strtok(readBuffSocket," \n");
						
						//case comparing 
						
						if(strcasecmp("add",token) == 0)
						{
							char * resultBuff = methodArithmetic(token, arthimeticBuff);
		
							writeCountSocket = write(msgsock, resultBuff, strlen(arthimeticBuff));
		 			
						}
						else if(strcasecmp("sub",token) == 0)
						{
							resultBuff = methodArithmetic(token, arthimeticBuff);
							
							writeCountSocket = write(msgsock, resultBuff, strlen(arthimeticBuff));
		 			
						}	
						else if(strcasecmp("mult",token) == 0)
						{
							resultBuff = methodArithmetic(token, arthimeticBuff);
		
							writeCountSocket = write(msgsock, resultBuff, strlen(arthimeticBuff));
		 			
						}
						else if(strcasecmp("div",token) == 0)
						{
							 resultBuff = methodArithmetic(token, arthimeticBuff);
		
							writeCountSocket = write(msgsock, resultBuff, strlen(arthimeticBuff));
		 			
						}
			
			  }while(readCountSocket != 0); 
		}
		else if(pid > 0)
		{
			noOfProcesses++;
			
			char writeBuff[70];
			int count = sprintf(writeBuff, "%d pid created\n", pid);
			write(STDOUT_FILENO, writeBuff, count);
		}
	}
}


