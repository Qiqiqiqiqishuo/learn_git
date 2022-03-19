#include "headOfServer.h"

#define BUFFER_SIZE 4097

int main(int argc, char *argv[])
{
    // $ ./zhen 192.168.119.128 1234
    ARGS_CHECK(argc, 3);
    int ret;
    char buffer[BUFFER_SIZE] = {0};

    struct sockaddr_in zhenAddr;
    bzero(&zhenAddr, sizeof(zhenAddr));
    zhenAddr.sin_family = AF_INET;
    zhenAddr.sin_addr.s_addr = inet_addr(argv[1]);
    zhenAddr.sin_port = htons(atoi(argv[2]));

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK_EXIT(sfd, -1, "socket");

    int reuseAddr = 1;
    ret = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(int));
    ERROR_CHECK(ret, -1, "setsockopt sfd reuseaddr(bind bind bing)");

    ret = bind(sfd, (struct sockaddr *)&zhenAddr, sizeof(zhenAddr));
    ERROR_CHECK_EXIT(ret, -1, "bind");

    ret = listen(sfd, 11);
    ERROR_CHECK_EXIT(ret, -1, "listen");

    puts("0");
    int nfd = accept(sfd, NULL, NULL);
    ERROR_CHECK_EXIT(nfd, -1, "accept");

    puts("1");
    dirLs(nfd);
    puts("2");
    dirPwd(nfd);
    puts("3");
    sleep(8);

    //    fd_set rdset;
    //
    //    while (1)
    //    {
    //        FD_ZERO(&rdset);
    //        FD_SET(nfd, &rdset);
    //        FD_SET(STDIN_FILENO, &rdset);
    //
    //        ret = select(nfd + 1, &rdset, NULL, NULL, NULL);
    //        ERROR_CHECK(ret, -1, "select");
    //
    //        if (FD_ISSET(nfd, &rdset))
    //        {
    //            bzero(buffer, BUFFER_SIZE);
    //            ret = recv(nfd, buffer, BUFFER_SIZE - 1, 0);
    //            if (ret == 0)
    //            {
    //                puts("zhen quit after qiang quit");
    //                break;
    //            }
    //            ERROR_CHECK(ret, -1, "read nfd");
    //
    //            if (buffer[ret - 1] == '\n')
    //            {
    //                buffer[ret - 1] = 0;
    //            }
    //            puts(buffer);
    //        }
    //
    //        if (FD_ISSET(STDIN_FILENO, &rdset))
    //        {
    //            bzero(buffer, BUFFER_SIZE);
    //            ret = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);
    //            if (ret == 0)
    //            {
    //                puts("I(zhen) quit");
    //
    //                strcpy(buffer, "zhen zhen quit\n");
    //                send(nfd, buffer, strlen(buffer), 0);
    //
    //                break;
    //            }
    //            ERROR_CHECK_EXIT(ret, -1, "read stdin");
    //
    //            time_t curtime = time(NULL);
    //            ERROR_CHECK_EXIT(curtime, -1, "time");
    //
    //            struct tm *sendtm = localtime(&curtime);
    //            ERROR_CHECK_EXIT(sendtm, NULL, "localtime");
    //
    //            char name_and_time[BUFFER_SIZE] = "zhen\t";
    //
    //            char sendtime[88] = {0};
    //            ret = sprintf(sendtime, "%d.%d.%d %02d:%02d:%02d\n",
    //                          sendtm->tm_year + 1900,
    //                          sendtm->tm_mon + 1,
    //                          sendtm->tm_mday,
    //                          sendtm->tm_hour,
    //                          sendtm->tm_min,
    //                          sendtm->tm_sec);
    //            ERROR_CHECK((ret < 0), 1, "sprintf to sendtime str");
    //
    //            strcat(name_and_time, sendtime);
    //
    //            send(nfd, name_and_time, strlen(name_and_time), 0);
    //            ERROR_CHECK(ret, -1, "send name and time");
    //
    //            send(nfd, buffer, strlen(buffer), 0);
    //            ERROR_CHECK(ret, -1, "send info");
    //        }
    //    }
    //
    //    ret = close(nfd);
    //    ERROR_CHECK_EXIT(ret, -1, "close nfd");
    //
    //    ret = close(sfd);
    //    ERROR_CHECK_EXIT(ret, -1, "close sfd");
    //
    return 0;
}