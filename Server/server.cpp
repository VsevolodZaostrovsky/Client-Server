#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#include "../Base/DataBase.h"

// Определимся с номером порта и другими константами.
#define PORT    5555
#define BUFLEN  1024

// Две вспомогательные функции для чтения/записи (см. ниже)
int   readFromClient(int fd, std::string &S);
int  writeToClient (int fd, std::string message);

int  main (void)
{
    DataBase DB;
    DB.addFromFile("Database.txt");

    int     i, err, opt=1;
    int     sock, new_sock;
    struct  sockaddr_in  addr;
    struct  sockaddr_in  client;
    char    buf[BUFLEN];
    socklen_t  size;

    // Создаем TCP сокет для приема запросов на соединение
    sock = socket (PF_INET, SOCK_STREAM, 0);
    if ( sock<0 ) {
        perror ("Server: cannot create socket");
        exit (EXIT_FAILURE);
    }
    // опция чтобы не ждать при повторном запуске сервера
    setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(const void*)&opt,sizeof(opt));

    // Заполняем адресную структуру и
    // связываем сокет с любым адресом
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    err = bind(sock,(struct sockaddr*)&addr,sizeof(addr));
    if ( err<0 ) {
        perror ("Server: cannot bind socket");
        exit (EXIT_FAILURE);
    }

    // Создаем очередь на 3 входящих запроса соединения
    err = listen(sock,3);
    if ( err<0 ) {
        perror ("Server: listen queue failure");
        exit(EXIT_FAILURE);
    }

    // Подготавливаем множества дескрипторов каналов ввода-вывода.
#define MAX_CLIENTS   100
    pollfd  act_set[MAX_CLIENTS];
    act_set[0].fd = sock;
    act_set[0].events = POLLIN;   // requested event
    act_set[0].revents = 0;       // returned event   
    int num_set = 1;
    std::string bufstr;



    // Основной бесконечный цикл проверки состояния сокетов
    while (1) {
        // Проверим, не появились ли данные в каком-либо сокете.
        // В нашем варианте ждем до фактического появления данных.
        DB.writeTo("Database.txt");
        int ret = poll (act_set, num_set, -1);
        if (ret < 0) {
            perror("Server: poll  failure");
            exit (EXIT_FAILURE);
        }
        if (ret > 0) {
           for (i = 0; i < num_set; i++) {
              if (act_set[i].revents & POLLIN) {
                  printf("get POLLIN at fd %d\n", act_set[i].fd);
                  act_set[i].revents &= ~POLLIN;
                  if (i==0) {
                      size = sizeof(client); 
                      new_sock = accept(act_set[i].fd,(struct sockaddr*)&client,&size);
                      if(new_sock < 0) {
                        perror("Server: accept failure");
                        exit (EXIT_FAILURE);
                      }
                      printf("new client at port %u\n", ntohs(client.sin_port));
                      if (num_set < MAX_CLIENTS) {
                         act_set[num_set].fd = new_sock;
                         act_set[num_set].events = POLLIN;
                         act_set[num_set].revents = 0;
                         num_set++;
                      } else {
                         printf("no more sockets for client");
                         close(new_sock);
                      }
                   } else {
                      // пришли данные в уже существующем соединени
                     // err = readFromClient(act_set[i].fd, buf);
                        bufstr = ""; 
                        err = readFromClient(act_set[i].fd, bufstr);
                        if ( err < 0 || bufstr == "stop") {
                          // ошибка или конец данных
                          printf("get stop\n");
                          close (act_set[i].fd);
                          // удаляем из множества сокетов 
                          if (i < num_set-1) {
                             act_set[i] = act_set[num_set - 1];
                             num_set--;
                             i--;
                          }
                      }
                        if(bufstr == "deactivate") { 
                          printf("stop server\n");
                          DB.writeTo("Database.txt");
                          return 1;
                      }
                      else {
                          // данные прочитаны нормально - отвечаем
                          if(bufstr != "stop") {
                            std::cout << "Get message: [" << bufstr << "]\n";
                            writeToClient(act_set[i].fd, DB.execute(bufstr,1).toString(BUFLEN));
                          }
                      }
                  }  
              }
           }
        }
    }
}

int readFromClient(int fd, std::string &S) {
    char  buf[BUFLEN];
    int len; char* ans; int nbytes;
    S = "";
    nbytes = recv(fd, &len, sizeof(int&), MSG_WAITALL); /* читаем длину сообщения */
    if (nbytes < 0) { perror ("Server: read failure"); }
    nbytes = write(fd, "ok", 3);
    if (nbytes < 0) { perror ("Server: write failure"); }
    std::cout << "len: " << len << "\n"; 
    ans = new char[len];
    nbytes = recv(fd, ans, len, MSG_WAITALL); /* читаем len байт */
    if (nbytes < 0) { perror ("Server: recv failure"); }
    std::string A(ans, len);
    S = A;
    delete [] ans;
    return 0;
}

int  writeToClient (int fd, std::string message)
{
    int   nbytes;
    char  buf[BUFLEN];
    std::string ans;

    int len = message.size();

    nbytes = write (fd, &len, sizeof(int&));
    if (nbytes < 0) { perror("write"); return -1; }
    
    nbytes = recv(fd, buf, 3, MSG_WAITALL);
    if (nbytes < 0) { perror ("Server: read failure"); }
    std::cout<<"Answer " << buf <<"\n";

    if(buf[0] == 'o' && buf[1] == 'k'){
        std::cout << "Write to client: [" << message << "]\n";
        nbytes = write (fd, message.c_str(), len);
    }
    if (nbytes < 0) { perror("write"); return -1; }
    if (message == "deactivate") return -1;
    if (message == "stop") return -1;  
    return 0;
}