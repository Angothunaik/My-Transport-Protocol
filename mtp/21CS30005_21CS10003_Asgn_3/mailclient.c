/**   
 *not written quit command 
 i have used DELE COMMAND TO DELETE THE MAIL AND USED RSET TO COMPLETLEY RESET THE DELETED MAIL TO MAIL
 * 

*/



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
#define MAX_LENGTH 80
#define MAX_LINES 50

void sendMail(int clientSocket)
{
    ssize_t bytesRead;
    char buffer[BUFFER_SIZE];
    char subject[80];
    // content = (char **)malloc(50 * sizeof(char *));
    char mail_from[20] = "yash@dhd";
    char mail_to[20] = "abhiram@ddj";

    while (1)
    {

        printf("\nC: ");
        fflush(stdout);
        char command[256];
        memset(command, 0, sizeof(command));

        fgets(command, sizeof(command), stdin);
        // getchar();
        command[strcspn(command, "\n")] = '\0';

        char code[4];
        if (strcasecmp(command, "HELLO") == 0)
        {
            memset(buffer, 0, sizeof(buffer));
            sprintf(buffer, "HELLO %s ", DOMAIN_NAME);
            //printf("\nC: %s", buffer);

            bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
            if (bytesRead < 0)
            {
                printf("\nError occured while reading from socket! SENding command to sever hello \n");
            }

            memset(buffer, '\0', sizeof(buffer));

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
            printf("enter mail:");
            // scanf(" %s", mail_from);
            fgets(mail_from, sizeof(mail_from), stdin);
            mail_from[strcspn(mail_from, "\n")] = '\0';
            // strcpy(from, mail_from);
            sprintf(buffer, "MAIL FROM:<%s>\n", mail_from);
            bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
            if (bytesRead < 0)
            {
                printf("\nError occurred while writing to socket! mail from \n");
                exit(0);
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
                printf("\nGo to next command...\n");
                memset(command, 0, sizeof(command));
                continue;
            }
            else
            {
                printf("\nError occurred! mail form\n");

                return;
            }
        }

        else if (strcasecmp(command, "RCPT TO") == 0)
        {
            bzero(buffer, 400);
            printf("\nEnter Recipient Email id: ");
            fgets(mail_to, sizeof(mail_to), stdin);
            mail_to[strcspn(mail_to, "\n")] = '\0';
            // strcpy(to, mail_to);
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
            }
            printf("\nC: %s \n", buffer); // DATA
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
                printf("C: FROM: %s \n", mail_from);
                printf("C: FROM: %s \n", mail_to);
                printf("\nReady to send header data!\n\n");
                printf("subject: ");
                fgets(subject, sizeof(subject), stdin);
                subject[strcspn(subject, "\n")] = '\0';
                printf("%s\n", subject);

                printf("enter mail body :\n");

                char content[50][80];
                strcpy(content[0], mail_from);
                strcpy(content[1], mail_to);
                strcpy(content[2], subject);
                int i = 3;
                // int j = 0;
                while (1)
                {
                    fgets(content[i], sizeof(content[i]), stdin); // Read a line of input
                    content[i][strcspn(content[i], "\n")] = '\0';
                    // Check if the line is the termination condition
                    if (strcmp(content[i], ".") == 0)
                    {
                        strcpy(content[i], ".");
                        break;
                    }

                    if (content[i][0] == '\0' || (content[i][0] != '\0' && strspn(content[i], " \t\n\r") == strlen(content[i])))
                    {
                        continue; // Skip this line
                    }
                    i++; // Move to the next row in the 2D array
                    if (i >= 80)
                    {
                        printf("Maximum number of lines reached.\n");
                        break;
                    }
                }

                char combinedContent[MAX_LINES * MAX_LENGTH] = ""; // Assuming MAX_LINES and MAX_LENGTH are defined constants
                for (int j = 0; j < i; j++)
                {
                    strcat(combinedContent, content[j]);
                    strcat(combinedContent, "\n"); // Add newline between each line of content
                }

                // Send the combined content to the server
                ssize_t bytesSent = send(clientSocket, combinedContent, strlen(combinedContent), 0);
                if (bytesSent < 0)
                {
                    perror("Error sending data to server");
                }
                else
                {
                    printf("Sent %zd bytes to server\n", bytesSent);
                }
            }
            else
            {
                printf("\nError occurred! data \n\n");
                return;
            }
        }
        else if (strcasecmp(command, "QUIT") == 0)
        {
            bzero(buffer, 400);
            bytesRead = send(clientSocket, command, 5, 0);
            if (bytesRead < 0)
            {
                printf("\nError occurred while writing to socket! rcpt to \n");
                return;
            }

            // printf("\nC: %s", buffer);
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
        else
        {

            printf("Invalid command: %s\n", command);
        }
    }
}

void manage_pop(int clientSocket)
{
    char command[256];
    char buffer[BUFFER_SIZE];
    while (1)
    {
        printf("\nC: ");
        fflush(stdout);
        // Read user input
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0'; // Remove trailing newline

        // Process user command
        switch (command[0])
        {
        case 'U':
            if (strncmp(command, "USER", 4) == 0)
            {
                ssize_t bytesRead;
                memset(buffer, '\0', sizeof(buffer));
                strncpy(buffer, command, sizeof(buffer) - 1);
                bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                if (bytesRead <= 0)
                {
                    // Connection closed or error occurred
                    break;
                }
                ssize_t byteRead;
                memset(buffer, '\0', sizeof(buffer));
                byteRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                if (byteRead <= 0)
                {
                    // Connection closed or error occurred
                    break;
                }

                printf("S: %s\n", buffer);
                if (strncmp(buffer, "ERR", 3) == 0)
                {
                    break;
                }
            } 
            case 'P':
            if (strncmp(command, "PASS", 4) == 0)
            {
                ssize_t bytesRead;
                memset(buffer, '\0', sizeof(buffer));
                strncpy(buffer, command, sizeof(buffer) - 1);
                bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                if (bytesRead <= 0)
                {
                    // Connection closed or error occurred
                    break;
                }
                ssize_t byteRead;
                memset(buffer, '\0', sizeof(buffer));
                byteRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                if (byteRead <= 0)
                {
                    // Connection closed or error occurred
                    break;
                }

                printf("S: %s\n", buffer);
                if (strncmp(buffer, "ERR", 3) == 0)
                {
                    break;
                } 
                else continue;
            }
            break;
        case 'S':
            if (strncmp(command, "STAT", 4) == 0)
            {
 

                ssize_t bytesRead;
                memset(buffer, '\0', sizeof(buffer));
                strncpy(buffer, command, sizeof(buffer) - 1);
                bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                if (bytesRead <= 0)
                {
                    
                    break;
                }

                ssize_t byteRead;
                memset(buffer, '\0', sizeof(buffer));
                byteRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                if (byteRead <= 0)
                {
                    
                    break;
                }

                printf("S: %s\n", buffer);
            }
            break;
        case 'L':
            if (strncmp(command, "LIST", 4) == 0)
            {
                
                ssize_t bytesRead;
                memset(buffer, '\0', sizeof(buffer));
                strcpy(buffer, command);
                bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                if (bytesRead <= 0)
                {
                    // Connection closed or error occurred
                    break;
                }
                // printf("%s\n",buffer);

                ssize_t byteRead;
                memset(buffer, '\0', sizeof(buffer));
                byteRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                if (byteRead <= 0)
                {
                    // Connection closed or error occurred
                    break;
                }

                char *token;

                token = strtok(buffer, "\n");
                int partIndex = 0;
                char parts[100][100];
                while (token != NULL && partIndex < 50)
                {
                    strcpy(parts[partIndex], token);
                    partIndex++;
                    token = strtok(NULL, "\n");
                }
                // char parts[100][100];
                for (int i = 0; i < partIndex; i++)
                {
                    printf("%s\n", parts[i]);
                }
                //*numParts = partIndex;
            }
            break;
        case 'R':
            if (strncmp(command, "RETR", 4) == 0)
            {
                // Handle RETR command
                // printf("Handling RETR command\n");
                ssize_t bytesRead;
                memset(buffer, '\0', sizeof(buffer));
                strcpy(buffer, command);
                bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                if (bytesRead <= 0)
                {
                    // Connection closed or error occurred
                    break;
                }
                ssize_t byteRead;
                memset(buffer, '\0', sizeof(buffer));
                byteRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                if (byteRead <= 0)
                {
                    // Connection closed or error occurred
                    break;
                }
                printf("%s\n", buffer);
            }
            else if (strncmp(command, "RSET", 4) == 0)
            {
                // Handle RSET command
                printf("Handling RSET command\n");

                ssize_t bytesRead;
                memset(buffer, '\0', sizeof(buffer));
                strcpy(buffer, command);
                bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                if (bytesRead <= 0)
                {
                    // Connection closed or error occurred
                    break;
                }
                ssize_t byteRead;
                memset(buffer, '\0', sizeof(buffer));
                byteRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                if (byteRead <= 0)
                {
                    // Connection closed or error occurred
                    break;
                }
                printf("%s\n", buffer);
            }
            break;
        case 'D':
            if (strncmp(command, "DELE", 4) == 0)
            {
                // Handle DELE command
                printf("Handling DELE command\n");
                ssize_t bytesRead;
                memset(buffer, '\0', sizeof(buffer));
                strcpy(buffer, command);
                bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                if (bytesRead <= 0)
                {
                    // Connection closed or error occurred
                    break;
                }
                ssize_t byteRead;
                memset(buffer, '\0', sizeof(buffer));
                byteRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                if (byteRead <= 0)
                {
                    // Connection closed or error occurred
                    break;
                }
                printf("%s\n", buffer);
            }
            break;
        default:
            printf("Invalid command. Please try again.\n");
            break;
        }
    }
}

int connectToPOP3Server(const char *serverIP, int pop3Port)
{
    int pop3Socket;
    struct sockaddr_in serverAddr;

    // Create socket
    pop3Socket = socket(AF_INET, SOCK_STREAM, 0);
    if (pop3Socket == -1)
    {
        perror("Socket creation failed");
        return -1;
    }

    // Prepare server address struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(pop3Port);
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);

    // Connect to server
    if (connect(pop3Socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Connection to POP3 server failed");
        return -1;
    }

    printf("C: Connectes to POP3 server\n");
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', sizeof(buffer));
    ssize_t byteRead = recv(pop3Socket, buffer, sizeof(buffer), 0);
    if (byteRead < 0)
    {
        printf("\nError occurred while reading from socket!\n");
    }
    printf("\nS: %s\n", buffer);
    return pop3Socket;
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
    printf("\nS: %s\n", buffer);

    return clientSocket;
}

int main(int argc, char *argv[])
{
    char buffer[BUFFER_SIZE];
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <server_IP> <smtp_port> <pop3_port> \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *serverIP = argv[1];
    int smtpPort = atoi(argv[2]);
    int pop3_port = atoi(argv[3]);
    // Main loop for user interaction
    while (1)
    {
        printf("Options:\n");
        printf("1. Manage Mail\n");
        printf("2. Send Mail\n");
        printf("3. Quit\n");

        int option;
        printf("Enter option: ");
        scanf("%d", &option);

        switch (option)
        {
        case 1:
            int pop3Socket = connectToPOP3Server(serverIP, pop3_port);
            if (pop3Socket == -1)
            {
                perror("Error connecting to POP3 server");
                exit(EXIT_FAILURE);
            }
            getchar();
            manage_pop(pop3Socket);
            close(pop3Socket);
            break;
        case 2:
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
        case 3:
            printf("Quitting...\n");
            exit(EXIT_SUCCESS);
        default:
            printf("Invalid option. Please try again.\n");
        }
    }
    return 0;
} 
