#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_LINE 1024

int main(void) {
  int sockfd, n;
  char recvline[MAX_LINE + 1];
  struct sockaddr_in servaddr;

  // Create a socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket");
    exit(1);
  }

  // Set up the server address
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(110); // POP3 port
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // localhost

  // Connect to the server
  if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
    perror("connect");
    exit(1);
  }

  // Receive the greeting from the server
  n = read(sockfd, recvline, MAX_LINE);
  if (n < 0) {
    perror("read");
    exit(1);
  }

  // Print the greeting
  printf("%s", recvline);

  // Send the username
  n = write(sockfd, "USER user\r\n", 10);
  if (n < 0) {
    perror("write");
    exit(1);
  }

  // Receive the response from the server
  n = read(sockfd, recvline, MAX_LINE);
  if (n < 0) {
    perror("read");
    exit(1);
  }

  // Print the response
  printf("%s", recvline);

  // Send the password
  n = write(sockfd, "PASS password\r\n", 13);
  if (n < 0) {
    perror("write");
    exit(1);
  }

  // Receive the response from the server
  n = read(sockfd, recvline, MAX_LINE);
  if (n < 0) {
    perror("read");
    exit(1);
  }

  // Print the response
  printf("%s", recvline);

  // Send the STAT command
  n = write(sockfd, "STAT\r\n", 6);
  if (n < 0) {
    perror("write");
    exit(1);
  }

  // Receive the response from the server
  n = read(sockfd, recvline, MAX_LINE);
  if (n < 0) {
    perror("read");
    exit(1);
  }

  // Print the response
  printf("%s", recvline);

  // Send the LIST command
  n = write(sockfd, "LIST\r\n", 6);
  if (n < 0) {
    perror("write");
    exit(1);
  }

  // Receive the response from the server
  n = read(sockfd, recvline, MAX_LINE);
  if (n < 0) {
    perror("read");
    exit(1);
  }

  // Print the response
  printf("%s", recvline);

  // Send the QUIT command
  n = write(sockfd, "QUIT\r\n", 6);
  if (n < 0) {
    perror("write");
    exit(1);
  }

  // Close the socket
  close(sockfd);

  return 0;
}