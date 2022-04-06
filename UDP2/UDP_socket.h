#ifndef UDP_SOCKET_H_INCLUDED
#define UDP_SOCKET_H_INCLUDED

#if defined WIN32
    #include <Winsock2.h>
#else
    #define closesocket close
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h> // for atoi()
#include <string.h>

#define VERBOSE 1 // se 1 stampa messaggi di errore, 0 non li stampa
#define BUFFERSIZE 512
#define ADDRSIZE 16
#define NAMESIZE 50

/**
 * Individua una socket
 */
typedef int UDPSocket;  /**< Id della socket, usato dalla Socket API */

/**
 * Termina l'uso delle Socket API
 * @warning Necessario per la compatibilit� con Windows
 */
void ClearWinSock();

/**
 *  Chiude una socket
 *
 * @param s una socket
 */
void CloseSocket(UDPSocket* s);

/**
 * Inizializza le Socket API
 * @warning Necessario per la compatibilit� con Windows
 *
 * @return 1 se successo, 0 se errore
 */
int InitWinSock();

/**
 *  Controlla se la socket � aperta
 *
 * @param s una socket
 * @return 1 se s � aperta, 0 se s non � aperta
 */
int IsOpenSocket(UDPSocket s);

/**
 * Crea una socket in attesa su una porta (lato server)
 *
 * @param port porta del server
 *
 * @return la socket creata
 */
UDPSocket ServerSocket(int port);

/**
 * Crea una socket di connessione (lato client)
 *
 * @return socket di connessione
 */
UDPSocket ClientSocket();

/**
 * Invia una stringa tramite una socket
 *
 * @param message stringa da inviare
 * @param address indirizzo IP del destinatario
 * @param port porta del destinatario
 * @param connectedSocket socket attraverso cui inviare la stringa
 *
 * @return 1 se la stringa � inviata nella sua interezza, 0 altrimenti
 */
int SendStringTo(char* message, const char* address, int port, UDPSocket connectedSocket);

/**
 * Riceve una stringa tramite una socket
 *
 * @param connectedSocket da cui leggere la stringa
 * @param address indirizzo IP del mittente
 * @param port porta del mittente
 *
 * @return stringa ricevuta
 */
char* RecvStringFrom(UDPSocket connectedSocket, char* address, int* port);

/**
 * Restituisce l'indirizzo IP associato a un nome host
 *
 * @param address un indirizzo IP
 *
 * @return nome host associato all'indirizzo IP
 */
char* Address2Name(char* address);

/**
 * Risolve un nome host
 *
 * @param name un nome host
 *
 * @return indirizzo IP
 */
char* Name2Address(char* name);


// ----- IMPLEMENTAZIONE ------



void ErrorHandler(const char *errorMessage) {
    if (VERBOSE)
        puts(errorMessage);
}

void ClearWinSock() {
    #if defined WIN32
    WSACleanup();
    #endif
}

void CloseSocket(UDPSocket* s) {
    closesocket(*s);
    *s = -1;
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

int IsOpenSocket(UDPSocket s) {
    if (s<0)
        return 0;
    return 1;
}

UDPSocket ServerSocket(int port) {
    UDPSocket sock;

    // CREAZIONE DELLA SOCKET
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        ErrorHandler("socket() failed");

    // COSTRUZIONE DELL'INDIRIZZO DEL SERVER
    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_port = htons(port);
    sad.sin_addr.s_addr = inet_addr("127.0.0.1");

    // BIND DELLA SOCKET
    if ((bind(sock, (struct sockaddr *)&sad, sizeof(sad))) < 0) {
        ErrorHandler("bind() failed");
        sock = -1;
    }

    return sock;
}

UDPSocket ClientSocket() {
    UDPSocket sock;

    // CREAZIONE DELLA SOCKET
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        ErrorHandler("socket() failed");
        sock = -1;
    }

    return sock;
}

int SendStringTo(char* message, const char* address, int port, UDPSocket connectedSocket) {
    // COSTRUZIONE DELL'INDIRIZZO DEL SERVER
    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_port = htons(port);
    sad.sin_addr.s_addr = inet_addr(address);

    // INVIARE DATI
    if (sendto(connectedSocket, message, strlen(message), 0, (struct sockaddr*)&sad, sizeof(sad)) != strlen(message)) {
        ErrorHandler("sendto() sent different number of bytes than expected");
        return -1;
    }

    return 1;
}

char* RecvStringFrom(UDPSocket connectedSocket, char* address, int* port) {

    struct sockaddr_in sad;
    int sadl;
    static char buf[BUFFERSIZE];
    unsigned int bytesRcvd;
    sadl = sizeof(sad);

    // RICEVERE DATI
    bytesRcvd = recvfrom(connectedSocket, buf, BUFFERSIZE-1, 0, (struct sockaddr*)&sad, &sadl);

    if (address!=NULL)
        strcpy(address, inet_ntoa(sad.sin_addr));
    if (port!=NULL)
        *port = ntohs(sad.sin_port);

    if (bytesRcvd <= 0) {
        ErrorHandler("recv() failed or connection closed prematurely");
        buf[0] = '\0';
        return buf;
    }

    buf[bytesRcvd] = '\0'; // Add \0 so printf knows where to stop
    return buf;
}

char* Address2Name(char* address) {
    struct in_addr addr;
    struct	hostent	*host;
    static char name[NAMESIZE];

    addr.s_addr	= inet_addr(address);
    host = gethostbyaddr((char*) &addr, 4, AF_INET);
    strcpy(name, host->h_name);

    return name;
}

char* Name2Address(char* name) {

    static char address[ADDRSIZE];
    struct hostent* host;

    host = gethostbyname(name);

    if	(host == NULL) {
        ErrorHandler("gethostbyname() failed");
        address[0] = '\0';
        return address;
    }

    struct in_addr* ina	= (struct in_addr*) host->h_addr_list[0];
    strcpy(address, inet_ntoa(*ina));

    return address;
}

#endif // UDP_SOCKET_H_INCLUDED
