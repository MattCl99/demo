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
#include <string.h> /* for memset() */
#define ECHOMAX 255
#define PORT 48000

void ErrorHandler(char *errorMessage) {
    printf(errorMessage);
}

void clearwinsock() {
    #if defined WIN32
	WSACleanup();
    #endif
}

int main(int argc, char *argv[]) {
	#if defined WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2 ,2), &wsaData);
    if (iResult != 0) {
        printf ("error at WSASturtup\n");
        return EXIT_FAILURE;
    }
    #endif

    int sock;
    struct sockaddr_in echoServAddr;
    struct sockaddr_in echoClntAddr;
    unsigned int cliAddrLen;
    char echoBuffer[ECHOMAX];
    int recvMsgSize;


// CREAZIONE DELLA SOCKET
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        ErrorHandler("socket() failed");


// COSTRUZIONE DELL'INDIRIZZO DEL SERVER
    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_port = htons(PORT);
    echoServAddr.sin_addr.s_addr = inet_addr("127.0.0.1");


// BIND DELLA SOCKET
    if ((bind(sock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr))) < 0)
        ErrorHandler("bind() failed");
    printf("Waiting for input...\n");

// RICEZIONE DELLA STRINGA ECHO DAL CLIENT
    while(1) {
        cliAddrLen = sizeof(echoClntAddr);
        recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0, (struct sockaddr*)&echoClntAddr, &cliAddrLen);
        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
        printf("Received: %s\n", echoBuffer);

// RINVIA LA STRINGA ECHO AL CLIENT
        if (sendto(sock, echoBuffer, recvMsgSize, 0, (struct sockaddr *)&echoClntAddr, sizeof(echoClntAddr)) != recvMsgSize)
            ErrorHandler("sendto() sent different number of bytes than expected");
    }

return 0;
} // main end
