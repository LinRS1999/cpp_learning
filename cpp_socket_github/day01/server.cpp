# include <iostream>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <cstring>
using namespace std;

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    // 将socket地址与文件描述符绑定
    bind(sockfd, (sockaddr*) &serv_addr, sizeof(serv_addr));
    // 使用listen函数监听这个socket端口，这个函数的第二个参数是listen函数的最大监听队列长度，系统建议的最大值SOMAXCONN被定义为128
    listen(sockfd, SOMAXCONN);

    struct sockaddr_in clnt_addr;
    // socklen_t是一种数据类型，它其实和int差不多
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    bzero(&clnt_addr, sizeof(clnt_addr));

    // accept需要写入客户端socket长度，所以需要定义一个类型为socklen_t的变量，并传入这个变量的地址
    int clnt_sockfd = accept(sockfd, (sockaddr*) &clnt_addr, &clnt_addr_len);

    // inet_ntoa将以网络字节序给出的网络主机地址 in 转换成以点分十进制表示的字符串（如127.0.0.1）
    // ntohs将一个16位数由网络字节顺序转换为主机字节顺序
    cout<<"new client fd "<<clnt_sockfd<<" IP: "<<inet_ntoa(clnt_addr.sin_addr)<<" Port: "<<ntohs(clnt_addr.sin_port)<<endl;

    return 0;
}