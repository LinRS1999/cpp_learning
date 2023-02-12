# include <sys/socket.h>
# include <arpa/inet.h>
# include <cstring>

int main() {
    // AF_INET表示使用IPv4，如果使用IPv6使用AF_INET6
    // SOCK_STREAM表示流格式、面向连接，多用于TCP。SOCK_DGRAM表示数据报格式、无连接，多用于UDP
    // 0表示根据前面的两个参数自动推导协议类型。设置为IPPROTO_TCP和IPPTOTO_UDP，分别表示TCP和UDP
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // sockaddr_in在头文件#include<netinet/in.h>或#include <arpa/inet.h>中定义
    struct sockaddr_in serv_addr;
    // bzero初始化这个结构体
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    // inet_addr()作用是将一个IP字符串转化为一个网络字节序的整数值
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // htons()作用是将端口号由主机字节序转换为网络字节序的整数值。(host to net)
    serv_addr.sin_port = htons(8888);

    // 指向sockaddr_in结构的指针也可以指向sockaddr，二者长度一样，都是16个字节，即占用的内存大小是一致的，因此可以互相转化
    // 把类型、ip地址、端口填充sockaddr_in结构体，然后强制转换成sockaddr，作为参数传递给系统调用函数
    // 不应操作sockaddr，sockaddr是给操作系统用的
    connect(sockfd, (sockaddr*)& serv_addr, sizeof(serv_addr));

    return 0;
}