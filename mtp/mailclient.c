#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <netdb.h>

#define BUFFER_SIZE 400
#define DOMAIN_NAME "server.smtp.com" 


void sendMail(int clientSocket)
{
    ssize_t bytesRead;
    char buffer[BUFFER_SIZE]; 
    char from[20]; 
    char to[20],subject[80],**content;
    content=(char **)malloc(50*sizeof(char *));

    while (1)
    {
        printf("\nC: ");
        fflush(stdout); 

        char command[256];
         fgets(command, sizeof(command), stdin); 
         command[strcspn(command, "\n")] = '\0';
        // getchar(); 

         //printf("%s",command);
        char code[4];
        if (strcasecmp(command, "HELLO") == 0)
        {
            memset(buffer,0,sizeof(buffer));
            sprintf(buffer,"HELLO %s ",DOMAIN_NAME); 
            printf("\nC: %s", buffer); 

            bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
            if (bytesRead < 0)
            {
                printf("\nError occured while reading from socket! SENding command to sever hello \n");
            }
            
            memset(buffer,'\0',sizeof(buffer)); 

            ssize_t byteRead = recv(clientSocket, buffer, sizeof(buffer), 0); 

            if (byteRead <= 0)
            {
                printf("\nError occured while reading from socket! receive command hello\n");
            }
            printf("S : %s\n", buffer); // 250 Hello domain

            
            code[0] = buffer[0];
            code[1] = buffer[1];
            code[2] = buffer[2];
            code[3] = '\0';

            if (strcmp(code, "250") == 0)
            {
                printf("\nGo to next command...\n\n");
            }
            else
            {
                printf("\nError occured! hello\n\n");
                return;
            }
            
        }

        else if (strcasecmp(command, "MAIL FROM") == 0)
        {
            bzero(buffer, 400);
            char mail_from[50]; 
            printf("enter mail:");
            scanf("%s", mail_from);
            strcpy(from,mail_from);
            sprintf(buffer, "MAIL FROM:<%s>\n", mail_from);
            bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
            if (bytesRead < 0)
            {
                printf("\nError occurred while writing to socket! mail from \n");
                return;
            }

            printf("\nC: %s", buffer);
            bzero(buffer, 400);
            bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesRead < 0)
            {
                printf("\nError occurred while reading from socket! mail form\n");
                return;
            }
                printf("S : %s\n", buffer); // 250 OK

                code[0] = buffer[0];
                code[1] = buffer[1];
                code[2] = buffer[2];
                code[3] = '\0';

                if (strcmp(code, "250") == 0)
                {
                    printf("\nGo to next command...\n\n"); 
                    continue;
                }
                else
                {
                    printf("\nError occurred! mail form\n\n");

                    return;
                }
            }

            else if (strcasecmp(command, "RCPT TO") == 0)
            {
                bzero(buffer, 400);
                printf("\nEnter Recipient Email id: ");
                char mail_to[50];
                scanf("%s", mail_to);
                strcpy(to,mail_to);
                sprintf(buffer, "RCPT TO:<%s>\n", mail_to);
                bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                if (bytesRead < 0)
                {
                    printf("\nError occurred while writing to socket! rcpt to \n");
                    return;
                }

                printf("\nC: %s", buffer);
                bzero(buffer, 400);
                bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                if (bytesRead < 0)
                {
                    printf("\nError occurred while reading from socket! rcpt to \n");
                    return;
                }
                printf("S : %s\n", buffer); // 250 OK

                
                code[0] = buffer[0];
                code[1] = buffer[1];
                code[2] = buffer[2];
                code[3] = '\0';

                if (strcmp(code, "250") == 0)
                {
                    printf("\nGo to next command...\n\n"); 
                    continue;
                }
                else
                {
                    printf("\nError occurred! rcpt to \n\n");
                    return;
                }
            }

            else if (strcasecmp(command, "DATA") == 0)
            {
                bzero(buffer, 400);
                sprintf(buffer, "DATA\n");
                bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                if (bytesRead < 0)
                {
                    printf("\nError occurred while writing to socket! data \n");
                    return; 

                printf("\nC: %s", buffer); // DATA
                bzero(buffer, 400);
                bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                if (bytesRead < 0)
                {
                    printf("\nError occurred while reading from socket! data \n");
                    return; 
                }
                printf("S : %s\n", buffer); 

                
                code[0] = buffer[0];
                code[1] = buffer[1];
                code[2] = buffer[2];
                code[3] = '\0';

                if (strcmp(code, "354") == 0)
                {
                    printf("\nReady to send header data!\n\n");
                    printf("subject: ");gets(subject);
                    printf("enter mail body :\n");
                    for(int i=0;i>=0;i++)
                    {
                        content[i]=(char *)malloc(80*sizeof(char));
                        gets(content[i]);
                        if(strcmp(content[i],".")==0)break;
                    }

                }
                else
                {
                    printf("\nError occurred! data \n\n");
                    return; 
                }
            }
            else if (strcasecmp(command, "QUIT") == 0)
            {{
                bzero(buffer, 400);
                bytesRead = send(clientSocket, command, 5, 0);
                if (bytesRead < 0)
                {
                    printf("\nError occurred while writing to socket! rcpt to \n");
                    return;
                }

                //printf("\nC: %s", buffer);
                bzero(buffer, 400);
                bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                if (bytesRead < 0)
                {
                    printf("\nError occurred while reading from socket! rcpt to \n");
                    return;
                }
                printf("S : %s\n", buffer); 

                
                code[0] = buffer[0];
                code[1] = buffer[1];
                code[2] = buffer[2];
                code[3] = '\0';

                if (strcmp(code, "221") == 0)
                {
                    printf("\nexiting\n\n"); 
                    exit(0);
                }
                else
                {
                    printf("\nError occurred! rcpt to \n\n");
                    return;
                }
            }
            }
            else
            {
                
                printf("Invalid command: %s\n", command);
            }
        }
    }
}
int connectToServer(const char *serverIP, int port)
{
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];
    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, serverIP, &serverAddr.sin_addr) <= 0)
    {
        perror("Invalid server address");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Connection to server failed");
        exit(EXIT_FAILURE);
    }

    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead < 0)
    {
        printf("\nError occured while reading from socket!\n");
    }
    printf("\nRecieved = %s\n", buffer);

    memset(buffer, '\0', sizeof(buffer));
    ssize_t byteRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (byteRead < 0)
    {
        printf("\nError occurred while reading from socket!\n");
    }
    printf("\nC: %s\n", buffer);

    return clientSocket;
}

int main(int argc, char *argv[])
{
    char buffer[BUFFER_SIZE];
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <server_IP> <smtp_port> \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *serverIP = argv[1];
    int smtpPort = atoi(argv[2]);
    // Main loop for user interaction
    while (1)
    {
        printf("Options:\n");
        // printf("1. Manage Mail\n");
        printf("1. Send Mail\n");
        printf("2. Quit\n");

        int option;
        printf("Enter option: ");
        scanf("%d", &option);

        switch (option)
        {
        case 1:
            int smtpSocket = connectToServer(serverIP, smtpPort);

            if (smtpSocket == -1)
            {
                perror("Error creating socket");
                exit(EXIT_FAILURE);
            } 
            getchar();
            sendMail(smtpSocket);
            close(smtpSocket);
            break;
        case 2:
            printf("Quitting...\n");
            exit(EXIT_SUCCESS);
        default:
            printf("Invalid option. Please try again.\n");
        }
    }
    return 0;
}