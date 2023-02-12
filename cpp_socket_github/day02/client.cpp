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
    // socket connect error: Connection refused
    errif(connect(sockfd, (sockaddr*) &serv_addr, sizeof(serv_addr)) == -1, "socket connect error");

    while (true) {
        char buf[1024];
        bzero(&buf, sizeof(buf));

        cin>>buf;
        // 发送缓冲区中的数据到服务器socket，返回已发送数据大小
        ssize_t write_bytes = write(sockfd, buf, sizeof(buf));
        if (write_bytes == -1) {
            cout<<"socket already disconnected, can't write any more"<<endl;
            break;
        }

        bzero(&buf, sizeof(buf));
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
        if (read_bytes > 0) {
            cout<<"message from server: "<<buf<<endl;
        } else if (read_bytes == 0) {
            cout<<"server socket disconnected"<<endl;
            break;
        } else if (read_bytes == -1) {
            close(sockfd);
            errif(true, "socket read error");
        }
    }

    close(sockfd);
    return 0;
}