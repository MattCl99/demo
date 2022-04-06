#include "../UDP_socket.h"
#include <ctype.h>

#define SERVERPORT 48000 // porta del server
#define SENDSIZE 2
#define RECVSIZE 50

// server

int main() {

    char clientAddress[ADDRSIZE];
    int clientPort;
    char sendBuf[SENDSIZE];
    char recvBuf[RECVSIZE];

    InitWinSock();

    UDPSocket socket = ServerSocket(SERVERPORT);

    while (1) {

        // attende una stringa
        strcpy(recvBuf, RecvStringFrom(socket, clientAddress, &clientPort));

        if (strlen(recvBuf)>1) {
        // ricevuto messaggio iniziale
            puts(recvBuf);
            printf("Messaggio ricevuto dal client con nome host %s\n", Address2Name(clientAddress));
            SendStringTo("OK", clientAddress, clientPort, socket);
        } else {
        // ricevuto carattere
            sprintf(sendBuf, "%c", toupper(recvBuf[0]));
            SendStringTo(sendBuf, clientAddress, clientPort, socket);
        }
    }
}
