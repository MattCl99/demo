#include "../TCP_socket.h"
#define SERVERPORT 5194 // porta del server
#define SERVERIP "127.0.0.1"

// client

int main() {

    char inputString[20];
    char outputString[50];

    InitWinSock();

    TCPSocket server = ClientSocket(SERVERIP, SERVERPORT);   // connessione al server

    if (!IsOpenSocket(server))      // errore!
        return -1;

    puts(StringRecv(server, 0));   //  ricezione stringa "connessione avvenuta"

    puts("Inserire un'operazione semplice tra due interi...");
    puts("formato: <intero> <intero> <operazione>");
    puts("l'operazione e' uno dei seguenti caratteri");
    puts("a: addizione");
    puts("s: sottrazione");
    puts("m: moltiplicazione");
    puts("d: divisione");
    fgets(inputString, sizeof(inputString), stdin);

    SendString(inputString, server);       // invio inputString
    strcpy(outputString, StringRecv(server, 0));   // ricezione outputString

    if (!strcmp(outputString, "TERMINE PROCESSO CLIENT"))
        puts("Chiusura connessione");
    else
        puts(outputString);

    CloseSocket(&server);       // chiusura connessione
    ClearWinSock();

    return(0);
}









