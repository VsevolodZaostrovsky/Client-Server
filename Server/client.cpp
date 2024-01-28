#include <iostream>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>

// Определимся с портом, адресом сервера и другими константами.
// В данном случае берем произвольный порт и адрес обратной связи
// (тестируем на одной машине).
#define  SERVER_PORT     5555
#define  SERVER_NAME    "127.0.0.1"
#define  BUFLEN          1024

// Две вспомогательные функции для чтения/записи 
int writeToServer (int fd);
int readFromServer(int fd, std::string &s);

int  main (void)
{
//    int i;
    int err;
    int sock;
    struct sockaddr_in server_addr;
    struct hostent    *hostinfo;

    // Получаем информацию о сервере по его DNS имени
    // или точечной нотации IP адреса.
    hostinfo = gethostbyname(SERVER_NAME);
    if ( hostinfo == NULL ) {
        fprintf (stderr, "Unknown host %s.\n",SERVER_NAME);
        exit (EXIT_FAILURE);
    }

    // Заполняем адресную структуру для последующего
    // использования при установлении соединения
    server_addr.sin_family = PF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr = *(struct in_addr*) hostinfo->h_addr;

    // Создаем TCP сокет.
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if ( sock<0 ) {
        perror ("Client: socket was not created");
        exit (EXIT_FAILURE);
    }

    // Устанавливаем соединение с сервером
    err = connect (sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if ( err<0 ) {
        perror ("Client:  connect failure");
        exit (EXIT_FAILURE);
    }
    fprintf (stdout,"Connection is ready\n");


    std::string s;
    // Обмениваемся данными
    while(1) {
        if (writeToServer(sock) < 0) break;
        if (readFromServer(sock, s) < 0) break;
    }

    // Закрываем socket
    close (sock);
    exit (EXIT_SUCCESS);

    fprintf (stdout, "The end\n");
}


int readFromServer(int fd, std::string &S) {
    char  buf[BUFLEN];
    int len; char* ans; int nbytes;
    S = " ";
    nbytes = recv(fd, &len, sizeof(int&), MSG_WAITALL); /* читаем длину сообщения */
    if (nbytes < 0) { perror ("Client: read failure"); }
    nbytes = write(fd, "ok", 3);
    if (nbytes < 0) { perror ("Client: write failure"); }
//    std::cout << "len: " << len << "\n"; 
    ans = new char[len];
    nbytes = recv(fd, ans, len, MSG_WAITALL); /* читаем len байт */
    if (nbytes < 0) { perror ("Client: recv failure"); }
    std::string A(ans, len);
    S = A;
    std::cout << "Servers replay: [" << A << "]\n";
    delete [] ans;
    return 0;
}

int  writeToServer (int fd)
{
    int   nbytes;
    char  buf[BUFLEN];
    std::string message;
    std::string ans;

    fprintf(stdout,"Send to server > ");
    std::getline(std::cin, message);
    while(message == "") {
        printf("Empty message. Try again\n");
        fprintf(stdout,"Send to server > ");
        std::getline(std::cin, message);
    }
    int len = message.size();

    nbytes = write (fd, &len, sizeof(int&));
    if (nbytes < 0) { perror("write"); return -1; }
    
/*    if(buf[0] != 'o') { 
        fprintf (stderr,"Error in WriteToSerUnlim\n");
        return -1; 
        } */
    nbytes = recv(fd, buf, 3, MSG_WAITALL);
    if (nbytes < 0) { perror ("Client: read failure"); }
    std::cout<< "Answer " << buf <<"\n";

    if(buf[0] == 'o' && buf[1] == 'k'){
        std::cout << "Write to server: [" << message << "]\n";
        nbytes = write (fd, message.c_str(), len);
    }
    if (nbytes < 0) { perror("write"); return -1; }
    if (message == "deactivate") return -1;
    if (message == "stop") return -1;  
    return 0;
}

