#ifndef TCP_SOCKET_H_INCLUDED
#define TCP_SOCKET_H_INCLUDED

#if defined WIN32
    #include <Winsock.h>
#else
    #define closesocket close
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h> // for atoi()

#define VERBOSE 1 // se 1 stampa messaggi di errore, 0 non li stampa
#define BUFFERSIZE 512
#define QLEN 10 // size of request queue

// Individua una socket
typedef struct {
    int id; 
    char address[20];   
    int port;   
} TCPSocket;


int InitWinSock();
void ClearWinSock();
int IsOpenSocket(TCPSocket s);
void CloseSocket(TCPSocket* s);
TCPSocket ServerSocket(int port, int maxQueueSize);
TCPSocket ServerAccept(TCPSocket ServerSocket);
TCPSocket ClientSocket(const char* address, int port);
char* StringRecv(TCPSocket connectedSocket, int BufferSize);
int SendString(char* message, TCPSocket connectedSocket);




void ErrorHandler(const char *errorMessage) {
    if (VERBOSE)
        puts(errorMessage);
}

void ClearWinSock() {
    #if defined WIN32
    WSACleanup();
    #endif
}

void CloseSocket(TCPSocket* s) {
    closesocket(s->id);
    s->id = -1;
}

int InitWinSock() {
    #if defined WIN32 // initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

    if (iResult != 0) {
        ErrorHandler("Error at WSAStartup()\n");
        return -1;
    }
    #endif

    return 1;
}

int IsOpenSocket(TCPSocket s) {
    if (s.id<0)
        return 0;
    return 1;
}

TCPSocket ServerSocket(int port, int maxQueueSize) {

    // CREAZIONE DELLA SOCKET
    TCPSocket MySocket;
    MySocket.port=port;
    strcpy(MySocket.address, "127.0.0.1");

    MySocket.id = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (MySocket.id < 0) {
        ErrorHandler("socket creation failed");
        MySocket.id = -1;
        return MySocket;
    }

    // ASSEGNAZIONE DI UN INDIRIZZO ALLA SOCKET
    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad)); // ensures that extra bytes contain 0
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = inet_addr(MySocket.address);
    sad.sin_port = htons(MySocket.port); 

    if (bind(MySocket.id, (struct sockaddr*) &sad, sizeof(sad)) < 0) {
        ErrorHandler("bind() failed");
        CloseSocket(&MySocket);
        return MySocket;
    }

    if (maxQueueSize<=0)
        maxQueueSize = QLEN;

    // SETTAGGIO DELLA SOCKET ALL'ASCOLTO
    if (listen (MySocket.id, maxQueueSize) < 0) {
        ErrorHandler("listen() failed");
        CloseSocket(&MySocket);
        return MySocket;
    }

    return MySocket;
}

TCPSocket ClientSocket(const char* address, int port) {

    // CREAZIONE DELLA SOCKET
    TCPSocket Csocket;
    strcpy(Csocket.address, address);
    Csocket.port = port;

    Csocket.id = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (Csocket.id < 0) {
        ErrorHandler("socket creation failed");
        Csocket.id = -1;
        return Csocket;
    }

    // COSTRUZIONE DELL INDIRIZZO DEL SERVER
    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = inet_addr(Csocket.address); // IP del server
    sad.sin_port = htons(Csocket.port); // Server port

    // CONNESSIONE AL SERVER
    if (connect(Csocket.id, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
        ErrorHandler( "Failed to connect" );
        CloseSocket(&Csocket);
        return Csocket;
    }

    return Csocket;
}

TCPSocket ServerAccept(TCPSocket ServerSocket) {
    // ACCETTARE UNA NUOVA CONNESSIONE
    struct sockaddr_in cad; // structure for the client address
    TCPSocket clientSocket;       // socket descriptor for the client
    int clientLen;          // the size of the client address

    clientLen = sizeof(cad); // set the size of the client address

    if ((clientSocket.id = accept(ServerSocket.id, (struct sockaddr *)&cad, &clientLen)) < 0) {
        ErrorHandler("accept() failed");
        clientSocket.id = -1;
        return clientSocket;
    }

    strcpy(clientSocket.address, inet_ntoa(cad.sin_addr));
    clientSocket.port = ntohs(cad.sin_port);

    return clientSocket;
}

int SendString(char* message, TCPSocket connectedSocket) {
    int stringLen = strlen(message); // Determina la lunghezza

    // INVIARE DATI
    if (send(connectedSocket.id, message, stringLen, 0) != stringLen) {
        ErrorHandler("send() sent a different number of bytes than expected");
        return -1;
    }

    return 1;
}

char* StringRecv(TCPSocket connectedSocket, int BufferSize) {

    if (BufferSize<=0)
        BufferSize=BUFFERSIZE;

    int bytesRcvd;
    static char buf[BUFFERSIZE]; // buffer for data from the server

    if ((bytesRcvd = recv(connectedSocket.id, buf, BUFFERSIZE - 1, 0)) <= 0) {
        ErrorHandler("recv() failed or connection closed prematurely");
        buf[0] = '\0';
        return buf;
    }

    buf[bytesRcvd] = '\0'; // Add \0 so printf knows where to stop
    return buf;
}

#endif // TCP_SOCKET_H_INCLUDED
