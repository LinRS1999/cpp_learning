#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include "util.h"

using namespace std;

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setnonblocking(int fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    errif(bind(sockfd, (sockaddr*) &serv_addr, sizeof(serv_addr) == -1), "socket bind error");

    errif(listen(sockfd, SOMAXCONN) == -1, "socket listen error");

    int epfd = epoll_create1(0);
    errif(epfd == -1, "epoll create error");

    struct epoll_event events[MAX_EVENTS], ev;
    bzero(&events, sizeof(events));

    bzero(&ev, sizeof(ev));
    ev.data.fd = sockfd;
    ev.events = EPOLLIN | EPOLLET;
    setnonblocking(sockfd);
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

    while (true) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        errif(nfds == -1, "epoll wait error");
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == sockfd) {
                struct sockaddr_in clnt_addr;
                bzero(&clnt_addr, sizeof(clnt_addr));
                socklen_t clnt_addr_len = sizeof(clnt_addr);

                int clnt_sockfd = accept(sockfd, (sockaddr*) &clnt_addr, &clnt_addr_len);
                errif(clnt_sockfd == -1, "socket accept error");
                cout<<"new client fd "<<clnt_sockfd<<" IP: "<<inet_ntoa(clnt_addr.sin_addr)<<" Port: "<<ntohs(clnt_addr.sin_port)<<endl;

                bzero(&ev, sizeof(ev));
                ev.data.fd = clnt_sockfd;
                ev.events = EPOLLIN | EPOLLET;
                setnonblocking(clnt_sockfd);
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sockfd, &ev);

            } else if (events[i].events & EPOLLIN) {
                char buf[READ_BUFFER];
                while (true) {
                    bzero(&buf, sizeof(buf));
                    ssize_t bytes_read = read(events[i].data.fd, buf, sizeof(buf));
                    if (bytes_read > 0) {
                        cout<<"message from client fd "<<events[i].data.fd<<": "<<buf<<endl;
                        write(events[i].data.fd, buf, sizeof(buf));
                    } else if (bytes_read == -1 && errno == EINTR) {
                        cout<<"continue reading"<<endl;
                        continue;
                    } else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
                        cout<<"finish reading once, errno: "<<errno<<endl;
                        break;
                    } else if (bytes_read == 0) {
                        cout<<"EOF, client fd "<<events[i].data.fd<<"disconnected"<<endl;
                        close(events[i].data.fd);
                        break;
                    }
                }
            } else {
                cout<<"something else happended"<<endl;
            }
        }
    }
    close(sockfd);
    return 0;
}