#include <stdio.h>
#include <string.h>

#if defined ( WIN32 ) || defined ( WIN64 )
	#include <winsock2.h>				   
	#define close closesocket	

	int init_winsock ( void )
	{
		WSADATA wsadata;
		if ( WSAStartup ( MAKEWORD ( 2, 2 ), & wsadata ) != 0 ) {
			return 0;
		}
		else {
			return 1;
		}
	}

#elif defined ( unix )
	#include <arpa/inet.h>
	#include <sys/socket.h>
	#define init_winsock ( ) 1
	#define closesocket close
	#define WSACleanup ( );
#endif

inline unsigned get_len ( char * str );
int send_mail ( char * subject, char * message, char * to, char * from, const char ip [ 17 ] );

#define IP "85.18.95.132" /* smtp.fastwebnet.it mail server */

#define FROM "example@notreply.com"
#define SUBJECT "Example"
#define MESSAGE "Just an example\n"\
				"Exampleeee\n"\
				"EEExampleeeee\n"\
				"End example"

int main ( int argc, char * argv [ ] ) {
	if ( argc < 2 ) // If the arguments are less than 1 it ends and it print the usage
	{
		printf ( "Usage: %s your@mail.com\n", argv [ 0 ] );
		return 0;
	}

	if ( send_mail ( SUBJECT, MESSAGE, argv [ 1 ], FROM, IP ) < 0 ) { // Send the mail or ends with an error
		puts ( "Error during the connection." );
		return -1;
	}
	
	return 0;
}

// SMTP port must be 25
#define PORT 25

/* Simple Mail Transfer Protocol
   HELO default 
   MAIL FROM: <> 
   RCPT TO: <> 
   DATA 
   From: <>
   Subject: 
   .
   QUIT */
   
#define _OPEN 	"HELO default\r\n"\
				"MAIL FROM: <"		

#define _RCPT  	">\r\nRCPT TO: <"   
#define _DATA  	">\r\nDATA\r\n"\
				"From: <"			

#define _SUB 	">\r\nSubject: " 	
#define _NEWLINE	"\r\n\r\n"

#define _CLOSE	"\r\n.\r\n"\
				"QUIT\r\n"			

// Get the lenght of a string in byte
inline unsigned get_len ( char * str ) {
	return ( unsigned ) ( strlen ( str ) * sizeof ( char ) );
}

// Send a mail and return 0 in case of success or -1 in case of error
int send_mail ( char * subject, char * message, char * to, char * from, const char server_ip [ 17 ] )
{
	int sock;
	struct sockaddr_in saddr; // Structure that contains the data to connect
	
	if ( ! init_winsock ( ) ) {
		return -1;
	}

	if ( ( sock = socket ( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) { // Open socket like TCP
		return -1;											  
	}
	
	saddr.sin_family = AF_INET;						 // Address family for IPv4
	saddr.sin_addr.s_addr = inet_addr ( server_ip ); // Convert the IP for the IN_ADDR structure
	saddr.sin_port = htons ( PORT );				 // Change the byte order in big-endian

	if ( connect ( sock, ( struct sockaddr * ) & saddr, sizeof ( saddr ) ) < 0 ) { // Open connection
		return -1;															
	}

	/* Send data for the protocol */
	size_t check_size = get_len ( _OPEN ); 					   // Get the lenght of the data in byte 
	if ( check_size != send ( sock, _OPEN, check_size, 0 ) ) { // Check that all the data was sent
		return -1;											   // Otherwise return an error
	}

	check_size = get_len ( from );
	if ( check_size != send ( sock, from, check_size, 0 ) ) {
		return -1;
	}
	
	check_size = get_len ( _RCPT );
	if ( check_size != send ( sock, _RCPT, check_size, 0 ) ) {
		return -1;
	}

	check_size = get_len ( to );
	if ( check_size != send ( sock, to, check_size, 0 ) ) {
		return -1;
	}

	check_size = get_len ( _DATA );
	if ( check_size != send ( sock, _DATA, check_size, 0 ) ) {
		return -1;
	}
	
	check_size = get_len ( from );
	if ( check_size != send ( sock, from, check_size, 0 ) ) {
		return -1;
	}
	
	check_size = get_len ( _SUB );
	if ( check_size != send ( sock, _SUB, check_size, 0 ) ) {
		return -1;
	}
	
	check_size = get_len ( subject );
	if ( check_size != send ( sock, subject, check_size, 0 ) ) {
		return -1;
	}
	
	check_size = get_len ( _NEWLINE );
	if ( check_size != send ( sock, _NEWLINE, check_size, 0 ) ) {
		return -1;
	}

	check_size = get_len ( message );
	if ( check_size != send ( sock, message, check_size, 0 ) ) {
		return -1;
	}
	
	check_size = get_len ( _CLOSE );
	if ( check_size != send ( sock, _CLOSE, check_size, 0 ) ) {
		return -1;
	}

	close ( sock ); // Close the socket
	WSACleanup ( );

	return 0;
}