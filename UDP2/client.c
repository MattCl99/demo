#include "../UDP_socket.h"
#include "vowels.h"

#define STRSIZE 100
#define SENDSIZE 10

// client

int main() {

    char serverName[NAMESIZE];
    char serverAddress[ADDRSIZE];
    int serverPort;

    char string[STRSIZE];
    char sendBuf[SENDSIZE];

    InitWinSock();

    puts("Inserire nome host del server:");
    scanf("%s", serverName);
    strcpy(serverAddress, Name2Address(serverName));

    puts("Inserire numero di porta del server:");
    scanf("%d", &serverPort);

    UDPSocket socket = ClientSocket();

    SendStringTo("Messaggio iniziale", serverAddress, serverPort, socket);
    puts(RecvStringFrom(socket, NULL, NULL));

    puts("Inserire una stringa:");
    fflush(stdin);
    fgets(string, sizeof(string), stdin);

    printf("Numero vocali: %d\n", countVowels(string));

    for (int i=0; i<countVowels(string); i++) {

        // invia vocale
        sprintf(sendBuf, "%c", nextVowel(string));
        SendStringTo(sendBuf, serverAddress, serverPort, socket);

        // stampa risposta
        puts(RecvStringFrom(socket, NULL, NULL));
    }

    CloseSocket(&socket);
    ClearWinSock();
}
