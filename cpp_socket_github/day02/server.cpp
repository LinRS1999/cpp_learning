#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include "util.h"

using namespace std;

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);
    // socket bind error: Address already in use
    errif(bind(sockfd, (sockaddr*) &serv_addr, sizeof(serv_addr)) == -1, "socket bind error");

    errif(listen(sockfd, SOMAXCONN) == -1, "socket listen error");

    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    bzero(&clnt_addr, sizeof(clnt_addr));

    int clnt_sockfd = accept(sockfd, (sockaddr*) &clnt_addr, &clnt_addr_len);
    errif(clnt_sockfd == -1, "socket accept error");

    cout<<"new client fd "<<clnt_sockfd<<" IP: "<<inet_ntoa(clnt_addr.sin_addr)<<" Port: "<<ntohs(clnt_addr.sin_port)<<endl;

    while (true) {
        char buf[1024];
        // ssize_t是有符号整型，在32位机器上等同与int，在64位机器上等同与long int
        // 从客户端socket读到缓冲区，返回已读数据大小
        ssize_t read_bytes = read(clnt_sockfd, buf, sizeof(buf));
        if (read_bytes > 0) {
            cout<<"message from client fd: "<<clnt_sockfd<<" "<<buf<<endl;
            // 将相同的数据写回到客户端
            write(clnt_sockfd, buf, sizeof(buf));
        } else if (read_bytes == 0) {
            cout<<"client fd "<<clnt_sockfd<<" disconnect"<<endl;
            // Linux系统的文件描述符理论上是有限的，在使用完一个fd之后，需要使用头文件<unistd.h>中的close函数关闭
            close(clnt_sockfd);
            break;
        } else if (read_bytes == -1) {
            close(clnt_sockfd);
            errif(true, "socket read error");
        }
    }

    close(sockfd);
    return 0;
}