#include "../TCP_socket.h"
#define SERVERPORT 5194 // porta del server

#define LOG 1 // se 1 mostra messaggi di connessione/disconnesione, se 0 altrimenti

// server

int main() {

    char inputString[20];
    char outputString[50];
    int operand1, operand2;
    char operation;
    int parse;

    InitWinSock();

    TCPSocket welcome = ServerSocket(SERVERPORT, 0);

    if (!IsOpenSocket(welcome))      // errore!
        return -1;

    while (1) {

        TCPSocket client = ServerAccept(welcome);    // connessione al client

        if (LOG)
            printf("Client Connesso %s:%d\n", client.address, client.port);

        SendString("Connessione avvenuta", client);    // invio stringa connessione avvenuta

        strcpy(inputString, StringRecv(client, 0));    // ricezione inputString

        parse = sscanf(inputString, "%d %d %c", &operand1, &operand2, &operation);

        if (parse==3)
            switch (operation) {
        case 'A': case 'a':
            sprintf(outputString, "Risultato dell'addizione: %d", operand1+operand2);
            break;

        case 'S': case 's':
            sprintf(outputString, "Risultato della sottrazione: %d", operand1-operand2);
            break;

        case 'M': case 'm':
            sprintf(outputString, "Risultato della moltiplicazione: %d", operand1*operand2);
            break;

        case 'D': case 'd':
            sprintf(outputString, "Risultato della divisione: %.2f", (float) operand1/operand2);
            break;

        default:
            parse = 0;
            break;
        }

        if (parse!=3)
            sprintf(outputString, "TERMINE PROCESSO CLIENT");

        SendString(outputString, client);    // invio outputString

        CloseSocket(&client);       // chiusura connessione

        if (LOG)
            printf("Client Disconnesso %s:%d\n", client.address, client.port);
    }
}
