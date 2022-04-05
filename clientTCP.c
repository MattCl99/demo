#if defined WIN32
#include <winsock.h>
#else
#include <string.h>
#include <unistd.h> //
#include <sys/types.h>
#include <sys/socket.h> //
#include <arpa/inet.h>  //
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
#endif

#include <stdio.h>
#include <stdlib.h>
#define BUFFERSIZE 512
#define PROTOPORT 27015 // Numero di porta di default

void errorhandler(char *error_message) {
printf("%s",error_message);
}

#define NO_ERROR 0
void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

int main(int argc, char *argv[]) {

// INIZIALIZZAZIONE WINSOCK
    #if defined WIN32
        WSADATA wsa_data;
        int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
        if (result != NO_ERROR) {
            printf("Error at WSAStartup()\n");
            return 0;
        }
    #endif

// CREAZIONE DELLA SOCKET
    int c_socket;
    c_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (c_socket < 0) {
        errorhandler("socket creation failed.\n");
        closesocket(c_socket);
        clearwinsock();
        return -1;
    }

// COSTRUZIONE DELL’INDIRIZZO DEL SERVER
    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP del server
    sad.sin_port = htons(27015); // Server port

// CONNESSIONE AL SERVER
    if (connect(c_socket, (struct sockaddr *)&sad, sizeof(sad))< 0) {
        errorhandler( "Failed to connect.\n" );
        closesocket(c_socket);
        clearwinsock();
        return -1;
    }

    char* input_string = "prova"; // Stringa da inviare
    int string_len = strlen(input_string); // Determina la lunghezza


// INVIARE DATI AL SERVER
    if (send(c_socket, input_string, string_len, 0) != string_len) {
        errorhandler("send() sent a different number of bytes than expected");
        closesocket(c_socket);
        clearwinsock();
        return -1;
    }


// RICEVERE DATI DAL SERVER
    int bytes_rcvd;
    int total_bytes_rcvd = 0;
    char buf[BUFFERSIZE];   // buffer for data from the server
    printf("Received: ");   // Setup to print the echoed string
    while (total_bytes_rcvd < string_len) {
        if ((bytes_rcvd = recv(c_socket, buf, BUFFERSIZE - 1, 0)) <= 0) {
            errorhandler("recv() failed or connection closed prematurely");
            closesocket(c_socket);
            clearwinsock();
            return -1;
        }

        total_bytes_rcvd += bytes_rcvd; // Keep tally of total bytes
        buf[bytes_rcvd] = '\0'; // Add \0 so printf knows where to stop
        printf("%s", buf);    // Print the echo buffer
    }

// CHIUSURA DELLA CONNESSIONE
    closesocket(c_socket);
    clearwinsock();
    printf("\n");    // Print a final linefeed


return 0;
} // main end
