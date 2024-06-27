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
#include <time.h> 


#define BUFFER_SIZE 400
#define DOMAIN_NAME "server.smtp.com"
#define MAX_LEN 1024

int checkUsername(const char inputUsername[])
{
    FILE *file = fopen("user.txt", "r");
    int usernameFound = 0;
    if (file == NULL)
    {
        perror("Error opening file");
        return -1;
    }
    char line[256];

    while (fgets(line, sizeof(line), file) != NULL)
    {
        char *token = strtok(line, " ");
        if (token != NULL)
        {
            if (strcmp(token, inputUsername) == 0)
            {
                usernameFound = 1;
                break;
            }
            else
                usernameFound = 0;
        }
    }
    fclose(file);
    return usernameFound;
}

void handleclient(int clientSocket)
{
    char buffer[BUFFER_SIZE];
    while (1)
    {

        // signal(SIGINT, handle_sigint);

        ssize_t bytesRead;
        memset(buffer, '\0', sizeof(buffer));
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        printf("%s\n", buffer);
        printf("Read %ld bytes\n", (long)bytesRead);

        if (bytesRead < 0)
        {
            printf("\nError occured while reading from socket!  command  \n");
        }
        else
        {
            if (strncmp(buffer, "HELLO", 5) == 0)
            {
                memset(buffer, '\0', sizeof(buffer));
               // printf("hello here\n");
                strcpy(buffer, "250 OK Hello " DOMAIN_NAME " \n");

                printf("%s\n", buffer);
                bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                if (bytesRead < 0)
                {
                    perror("Error occured while writing to socket! send hello");
                    return;
                }
                printf("success\n");
                continue;
            }

            if (strstr(buffer, "MAIL FROM") != NULL)
            {

                char *start = strchr(buffer, '<');
                char *end = strchr(buffer, '@');
                char mail_from[100];
                if (start != NULL && end != NULL)
                {

                    strncpy(mail_from, start + 1, end - start - 1);
                    mail_from[end - start - 1] = '\0';
                }

                int check = checkUsername(mail_from);
                if (check == 1)
                {
                    memset(buffer, '\0', sizeof(buffer));
                    strcpy(buffer, "250 OK ");
                    strcat(buffer, mail_from);
                    strcat(buffer, DOMAIN_NAME);
                    strcat(buffer, ".....Sender ok");
                    bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                    if (bytesRead < 0)
                    {
                        perror("Error occurred while writing to socket! mail from");
                        return;
                    }
                }
                else
                {

                    memset(buffer, '\0', sizeof(buffer));
                    strcpy(buffer, "500 Syntax error, command unrecognized\r\n");
                    bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                    if (bytesRead < 0)
                    {
                        perror("Error occurred while writing to socket! mail from");
                        return;
                    }
                }
            }
            else if (strstr(buffer, "RCPT TO") != NULL)
            {

                char *start = strchr(buffer, '<');
                char *end = strchr(buffer, '@');
                char mail_to[100];
                if (start != NULL && end != NULL)
                {
                    strncpy(mail_to, start + 1, end - start - 1);
                    mail_to[end - start - 1] = '\0';
                }

                int check = checkUsername(mail_to);
                if (check == 1)
                {
                    memset(buffer, '\0', sizeof(buffer));
                    strcpy(buffer, "250 root ....Recptor ok\n");
                    bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                    if (bytesRead < 0)
                    {
                        perror("Error occurred while writing to socket! rcpt to");
                        return;
                    }
                }
                else
                {

                    memset(buffer, '\0', sizeof(buffer));
                    strcpy(buffer, "500 Syntax error, command unrecognized\r\n");
                    bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                    if (bytesRead < 0)
                    {
                        perror("Error occurred while writing to socket! recpt to ");
                        return;
                    }
                }
            }
            else if (strstr(buffer, "DATA") != NULL)
            {

                memset(buffer, '\0', sizeof(buffer));
                strcpy(buffer, "354 4 Enter mail, end with . on a line by itself\n");
                bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                if (bytesRead < 0)
                {
                    perror("Error occurred while writing to socket! send data command");
                    return;
                }
                char mail_f[20];
                char mail_r[20];
                char *mail_t;
                char header[100];

                char content[50][80]; // Assuming maximum 50 lines of 80 characters each
                int i = 0;
                ssize_t bytesReceived;

                // Buffer to store received data
                char receivedData[MAX_LEN];

                // Receive data from the server
                ssize_t byteReceived;
                byteReceived = recv(clientSocket, receivedData, sizeof(receivedData), 0);

                printf("Received data from server: %s\n", receivedData);

                if (byteReceived < 0)
                {
                    perror("Error receiving data from server");
                }

                char *token = strtok(receivedData, "\n");
                // int i = 0;
                while (token != NULL && i < 80)
                {
                    strcpy(content[i], token);
                    i++;
                    token = strtok(NULL, "\n");
                }

                strcpy(mail_f, content[0]);
                strcpy(mail_r, content[1]);
                strcpy(header, content[2]); 
                char to[20]; 

                strcpy(to, content[1]);
                printf("Sender Email: %s\n", mail_f);
                printf("Recipient Email: %s\n", mail_r);
                printf("Header: %s\n", header);

                mail_t = strtok(mail_r, "@"); // Assuming header can be up to 100 characters
                char *path;
                path = (char *)malloc(strlen(mail_t) + strlen("./mymailbox") + 2);
                sprintf(path, "./%s/mymailbox", mail_t);

                printf("\n %s", path);

                FILE *file = fopen(path, "a"); // Open file for writing
                if (file == NULL)
                {
                    perror("Error opening file");
                    return;
                }
                fprintf(file, "FROM: %s\n", mail_f);
                fprintf(file, "TO: %s\n", to);
                fprintf(file, "Subject: %s\n", header);

                time_t now;
                time(&now);
                struct tm *localTime = localtime(&now);

                // Format the time
                char timeStr[100];
                strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localTime);

                // Write received time to file
                fprintf(file, "Received: %s\n", timeStr); 


                for (int j = 3; j < i - 1; j++)
                {

                    fprintf(file, "%s\n", content[j]);
                }
                fprintf(file, ". \n");

                printf("\n %s", mail_f);
                printf("\n %s", mail_t);
                printf("\n %s", header);

                fclose(file); // Close file
            }

            else if (strstr(buffer, "QUIT") != NULL)
            {

                memset(buffer, '\0', sizeof(buffer));
                strcpy(buffer, "221 Syntax error, command unrecognized\r\n");
                bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                printf("quitiing\n");
                if (bytesRead < 0)
                {
                    perror("Error occurred while writing to socket!");
                    return;
                }
                close(clientSocket);
                exit(0);
            }

            else
            {

                memset(buffer, '\0', sizeof(buffer));
                strcpy(buffer, "500 Syntax error, command unrecognized\r\n");
                bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                if (bytesRead < 0)
                {
                    perror("Error occurred while writing to socket!");
                    return;
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char buffer[BUFFER_SIZE];
    int myPort = atoi(argv[1]);

    struct sockaddr_in serverAddr, clientAddr;
    int clientlen;

    // Create the socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        perror("Cannot create socket");
        exit(EXIT_FAILURE);
    }

    printf("[+] Server Socket is created.\n");

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(myPort);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Error in binding");
        exit(EXIT_FAILURE);
    }

    printf("[+] Bind to port %d\n", myPort);

    if (listen(serverSocket, 5) == 0)
    {
        printf("[+] Listening....\n");
    }
    else
    {
        perror("Error in Listening");
        exit(EXIT_FAILURE);
    }

    printf("\n[+] SMTP server listening on port %d...\n", myPort);

    while (1)
    {
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientlen);
        if (clientSocket < 0)
        {
            perror("Error accepting connection");
            exit(EXIT_FAILURE);
        }

        printf("\n[+] Connection accepted from %s:%d\n\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        strcpy(buffer, "<client connects to SMTP PORT");

        write(clientSocket, buffer, sizeof(buffer));

        if (fork() == 0)
        {
            close(serverSocket);

            memset(buffer, '\0', sizeof(buffer));

            sprintf(buffer, "220 <%s> Service Ready", DOMAIN_NAME);
            send(clientSocket, buffer, strlen(buffer), 0);

            handleclient(clientSocket);

            close(clientSocket);
            exit(0);
        }

        close(clientSocket);
    }

    close(serverSocket);

    return 0;
}