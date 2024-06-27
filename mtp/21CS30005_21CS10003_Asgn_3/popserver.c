#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define MAX_LINE_LENGTH 80
#define MAX_MESSAGES 50

typedef struct
{
    int number;
    int size;
} MessageInfo;
int password_check(char pas[])
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
            if (strcmp(token, pas) == 0)
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

void handle_client(int clientSocket)
{
    char buffer[BUFFER_SIZE];
    int count = 0;
    char deletedMessage[BUFFER_SIZE][BUFFER_SIZE];
    while (1)
    {
        ssize_t bytesRead;
        memset(buffer, '\0', sizeof(buffer));
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0)
        {
            // Connection closed or error occurred
            break;
        }
        printf("Client sent: %s\n", buffer);

        // Process POP3 commands
        switch (buffer[0])
        {
        case 'U':
            if (strncmp(buffer, "USER", 4) == 0)
            {
                char command[100];
                strcpy(command, buffer);
                ssize_t bytesRead;
                memset(buffer, '\0', sizeof(buffer));
                strncpy(buffer, command + 5, sizeof(buffer) - 1);
                //printf("%s\n", buffer);
                int checks = checkUsername(buffer);
                if (checks == 0)
                {
                    memset(buffer, '\0', sizeof(buffer));
                    sprintf(buffer, "OK please enter password ");

                    bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                    if (bytesRead < 0)
                    {
                        perror("Error occured while writing to socket! send hello");
                        return;
                    }
                }
                else
                {
                    memset(buffer, '\0', sizeof(buffer));
                    sprintf(buffer, "ERR not found");

                    bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                    if (bytesRead < 0)
                    {
                        perror("Error occured while writing to socket! send hello");
                        return;
                    }
                    if (checks == 0)
                    {
                        memset(buffer, '\0', sizeof(buffer));
                        sprintf(buffer, "OK valid login ");

                        bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                        if (bytesRead < 0)
                        {
                            perror("Error occured while writing to socket! send hello");
                            return;
                        }
                    }
                }
            }
            break;
        case 'P':
            if (strncmp(buffer, "PASS", 4) == 0)
            {
                char command[100];
                strcpy(command, buffer);
                ssize_t bytesRead;
                memset(buffer, '\0', sizeof(buffer));
                strncpy(buffer, command + 5, sizeof(buffer) - 1);
               // printf("%s\n", buffer);
                int checks = password_check(buffer);
                if (checks == 0)
                {
                    memset(buffer, '\0', sizeof(buffer));
                    sprintf(buffer, "OK VALID PASSWORD ");

                    bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                    if (bytesRead < 0)
                    {
                        perror("Error occured while writing to socket! send hello");
                        return;
                    }
                }
                else
                {
                    memset(buffer, '\0', sizeof(buffer));
                    sprintf(buffer, "ERR not found");

                    bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                    if (bytesRead < 0)
                    {
                        perror("Error occured while writing to socket! send hello");
                        return;
                    }
                }
            }
            break;
        case 'S':
            if (strncmp(buffer, "STAT", 4) == 0)
            {
                // Handle STAT command
                // printf("Handling STAT command\n");

                FILE *file;
                char line[MAX_LINE_LENGTH];
                int num_messages = 0;
                int mailbox_size = 0;
                MessageInfo messages[MAX_MESSAGES];

                // Open mailbox file for reading
                file = fopen("./abhiram/mymailbox", "r");
                if (file == NULL)
                {
                    perror("Error opening mailbox file");
                    return;
                }

                // Read mailbox file line by line
                while (fgets(line, sizeof(line), file) != NULL)
                {
                    if (strncmp(line, "FROM:", 5) == 0)
                    {
                        num_messages++;
                    }
                    mailbox_size += strlen(line);
                }

                printf("%d  , %d ", num_messages, mailbox_size);

                memset(buffer, '\0', sizeof(buffer));
                sprintf(buffer, "OK <num_messages : %d> <mailbox_size: %d> ", num_messages, mailbox_size);

                bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                if (bytesRead < 0)
                {
                    perror("Error occured while writing to socket! send hello");
                    return;
                }

                continue;

                fclose(file);
            }

            break;
        case 'L':
            if (strncmp(buffer, "LIST", 4) == 0)
            {
                // Handle LIST command
                // printf("Handling LIST command\n");
                FILE *file;
                char line[MAX_LINE_LENGTH];
                int num_messages = 0;
                int mailbox_size = 0;
                MessageInfo messages[MAX_MESSAGES];

                // Open mailbox file for reading
                file = fopen("./abhiram/mymailbox", "r");
                if (file == NULL)
                {
                    perror("Error opening mailbox file");
                    return;
                }

                // Read mailbox file line by line
                int message_number = 0;
                while (fgets(line, sizeof(line), file) != NULL)
                {
                    if (strncmp(line, "FROM:", 5) == 0)
                    {
                        num_messages++;
                        message_number++;
                        messages[message_number - 1].number = message_number;
                        messages[message_number - 1].size = strlen(line);
                        mailbox_size += strlen(line);
                    }
                    else
                    {
                        messages[message_number - 1].size += strlen(line);
                        mailbox_size += strlen(line);
                    }
                }

                // char combined_string[1001];

                char combined_string[1001]; // Allocate enough space for your string
                sprintf(combined_string, "+OK %d messages, %d size\n", num_messages, mailbox_size);

                // Concatenate messages into one string
                for (int i = 0; i < num_messages; i++)
                {
                    char message_str[100]; // Allocate enough space for each message
                    snprintf(message_str, sizeof(message_str), "%d %d\n", messages[i].number, messages[i].size);
                    strcat(combined_string, message_str);
                }

                memset(buffer, '\0', sizeof(buffer));
                // printf("hello here\n");
                sprintf(buffer, "%s ", combined_string);

                // printf("%s\n", buffer);
                bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                if (bytesRead < 0)
                {
                    perror("Error occured while writing to socket! send hello");
                    return;
                }
                // printf("success\n");
                continue;

                // Close the file
                fclose(file);

                // Print the number of messages and the mailbox size
                printf("+OK %d messages\n", num_messages);
                for (int i = 0; i < num_messages; i++)
                {
                    printf("%d %d\n", messages[i].number, messages[i].size);
                }
            }
            break;
        case 'R':
            if (strncmp(buffer, "RETR", 4) == 0)
            {
                // Handle RETR command
                // printf("Handling RETR command\n");

                char *num = buffer;
                while (*num && !isdigit(*num))
                {
                    num++;
                }
                int requested_message_number = atoi(num);
                // printf("%d\n", requested_message_number);

                FILE *file;
                char line[MAX_LINE_LENGTH];
                int message_number = 0;
                int message_found = 0;
                int in_body = 0; // Flag to indicate if we're in the message body
                char message_body[MAX_LINE_LENGTH * 12];

                memset(message_body, '\0', sizeof(message_body));

                // Open mailbox file for reading
                file = fopen("./abhiram/mymailbox", "r");
                if (file == NULL)
                {
                    perror("Error opening mailbox file");
                    return;
                }

                // Read mailbox file line by line
                while (fgets(line, sizeof(line), file) != NULL)
                {
                    if (strncmp(line, "FROM:", 5) == 0)
                    {
                        // strcat(message_body, line);
                        // Found a new message, increment message number
                        message_number++;

                        // Check if this is the requested message
                        if (message_number == requested_message_number)
                        {
                            strcat(message_body, line);
                            while (fgets(line, sizeof(line), file) != NULL)
                            {
                                strcat(message_body, line);
                                if (strncmp(line, ".", 1) == 0)
                                {
                                    break;
                                }
                            }

                            message_found = 1;

                            break; // Stop searching for messages
                        }
                    }
                }

                // Close the file
                fclose(file);

                if (!message_found)
                {
                    // If message not found, send error response
                    memset(buffer, '\0', sizeof(buffer));
                    sprintf(buffer, "MESSAGE IS NOT FOUND ");
                    send(clientSocket, buffer, strlen(buffer), 0);
                }
                else
                {

                    memset(buffer, '\0', sizeof(buffer));
                    // printf("hello here\n");
                    sprintf(buffer, "%s ", message_body);

                    // printf("%s\n", buffer);
                    bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                    if (bytesRead < 0)
                    {
                        perror("Error occured while writing to socket! send hello");
                        return;
                    }
                    // printf("success\n");
                    continue;
                }
            }
            else if (strncmp(buffer, "RSET", 4) == 0)
            {
                

                FILE *file = fopen("./abhiram/mymailbox", "a"); // Open the file in append mode
                if (file == NULL)
                {
                    perror("Error opening mailbox file");
                    return;
                }
                // printf("%s\n",deletedMessage[count]);
                //  Write each message from the deletedMessages array to the file
                for (int i = 0; i <= count; i++)
                {
                    fprintf(file, "%s\n", deletedMessage[i]);
                }
                for (int i = 0; i < BUFFER_SIZE; i++)
                {
                    for (int j = 0; j < BUFFER_SIZE; j++)
                    {
                        deletedMessage[i][j] = '\0';
                    }
                }

                fclose(file);
                memset(buffer, '\0', sizeof(buffer));
                // printf("hello here\n");
                sprintf(buffer, "+OK MailDrop has done");

                // printf("%s\n", buffer);
                bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                if (bytesRead < 0)
                {
                    perror("Error occured while writing to socket! send hello");
                    return;
                }
            }
            break;
        case 'D':
            if (strncmp(buffer, "DELE", 4) == 0)
            {
                
                FILE *inputFile = fopen("./abhiram/mymailbox", "r");    // Open input mailbox file
                FILE *outputFile = fopen("./abhiram/tempmailbox", "w"); // Open temporary mailbox file for writing deleted messages

                if (inputFile == NULL || outputFile == NULL)
                {
                    perror("Error opening files");
                    return;
                }

                char mailbox[BUFFER_SIZE];
                if (count == 0)
                {

                    for (int i = 0; i < BUFFER_SIZE; i++)
                    {
                        for (int j = 0; j < BUFFER_SIZE; j++)
                        {
                            deletedMessage[i][j] = '\0';
                        }
                    }
                }
                count++;

                char line[BUFFER_SIZE]; // Assuming maximum size for each line

                int message_number = 0;
                char *num = buffer;
                while (*num && !isdigit(*num))
                {
                    num++;
                }
                int requested_message_number = atoi(num); // Message number starts from 1

                int message_found = 0;

                // Read the mailbox content

               
                while (fgets(line, sizeof(line), inputFile) != NULL)
                {
                    // Check if this is the start of a new message
                    if (strncmp(line, "FROM:", 5) == 0)
                    {
                        message_number++;

                        // If it's not the message to delete, write it to the output mailbox file
                        if (message_number != requested_message_number)
                        {
                            fputs(line, outputFile); // Write "FROM:" line
                            // Write the rest of the message until the end
                            while (fgets(line, sizeof(line), inputFile) != NULL && strncmp(line, ".", 1) != 0)
                            {
                                fputs(line, outputFile);
                            }
                            if (message_number != requested_message_number)
                            {
                                fputs(".\n", outputFile); // End of the message
                            }
                        }
                        else
                        {                                        // If it's the message to delete, store it in deletedMessage string
                            strcat(deletedMessage[count], line); // Store "FROM:" line
                            // Store the rest of the message until the end
                            while (fgets(line, sizeof(line), inputFile) != NULL && strncmp(line, ".", 1) != 0)
                            {
                                strcat(deletedMessage[count], line);
                            }
                            strcat(deletedMessage[count], ".\n"); // End of the message
                            message_found = 1;
                        }
                    }
                }

                fclose(inputFile);
                fclose(outputFile);

                if (!message_found)
                {
                    printf("Requested message not found.\n");
                    return;
                }

                printf("+OK Message marked for deletion\n");

                // printf("Deleted message:\n%s\n", deletedMessage[requested_message_number]); // Print the deleted message

                // Rename temporary mailbox file to overwrite the original mailbox file
                if (remove("./abhiram/mymailbox") == 0)
                {
                    printf("File  deleted successfully.\n");
                }
                else
                {
                    printf("Unable to delete the file .\n");
                }
                if (rename("./abhiram/tempmailbox", "./abhiram/mymailbox") != 0)
                {
                    perror("Error renaming the file");
                    // return;
                }

                memset(buffer, '\0', sizeof(buffer));
                // printf("hello here\n");
                sprintf(buffer, "+OK Message marked for deletion");

                bytesRead = send(clientSocket, buffer, strlen(buffer), 0);
                if (bytesRead < 0)
                {
                    perror("Error occured while writing to socket! send hello");
                    return;
                }
            } 
            break;
        default:
            printf("Unknown command\n");
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <pop3_port>\n", argv[0]);
        return 1;
    }

    int pop3_port = atoi(argv[1]);

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("Socket creation failed");
        return 1;
    }
    printf("[+] Server Socket is created.\n");

    // Prepare server address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(pop3_port);

    // Bind socket to address and port
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Binding failed");
        return 1;
    }
    printf("[+] Bind to port \n");
    // Listen for connections
    if (listen(server_socket, MAX_CLIENTS) < 0)
    {
        perror("Listening failed");
        return 1;
    }

    printf("POP3 server listening on port %d\n", pop3_port);

    // Accept incoming connections
    // char buufer[BUFFER_SIZE];
    while (1)
    {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket < 0)
        {
            perror("Accepting connection failed");
            continue;
        }
        char buffer[BUFFER_SIZE];
        memset(buffer, '\0', sizeof(buffer));
        strcpy(buffer, "+OK POP3 server ready\r\n");
        ssize_t bytesRead = send(client_socket, buffer, strlen(buffer), 0);

        printf("New client connected\n");

        if (fork() == 0)
        {
            close(server_socket);
            handle_client(client_socket);

            close(client_socket);
            exit(0);
        }

        // Handle client in a separate function
        // handle_client(client_socket);
        close(client_socket);
    }

    // Close server socket
    close(server_socket);

    return 0;
}
