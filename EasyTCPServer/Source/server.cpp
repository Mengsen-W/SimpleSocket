// 防止头文件的宏定义矛盾
#define WIN32_LEAN_AND_MEAN
// 定义转换函数宏
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <windows.h>

#include <iostream>

// 添加链接库
// #pragma comment(lib, "ws2_32.lib")

int main(void) {
  WORD ver = MAKEWORD(2, 2);
  WSADATA data;

  // 启动 Socket
  if (-1 == WSAStartup(ver, &data)) {
    std::cout << "Error Startup" << std::endl;
    return -1;
  }

  // ---------------------

  // 用Socket API 建立一个简单的 TCP 服务器
  // 1. 建立 socket 环境
  SOCKET _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  // 2. bind 绑定用于接收客户端连接的网路端口

  // sockaddr_in 的数据结构便于在代码中填写
  sockaddr_in _sin = {};
  // 绑定协议类型
  _sin.sin_family = AF_INET;
  // 绑定端口号
  _sin.sin_port = htons(4567);  // host to net unsign short
  // 绑定端口号
  _sin.sin_addr.S_un.S_addr =
      INADDR_ANY;  // inet_addr("127.0.0.1") INADDR_ANY 不限定默认
  // 绑定全部
  if (SOCKET_ERROR == bind(_socket, (sockaddr*)&_sin, sizeof(_sin)))
    std::cout << "Error bind to Netport." << std::endl;

  // 3. listen 监听网络端口 因为由握手的过程
  if (SOCKET_ERROR == listen(_socket, 5))
    std::cout << "Error listen to Netport." << std::endl;

  // 4. accept 等待接收客户端连接
  sockaddr_in clientAddr = {};
  int nAddrLend = sizeof(clientAddr);
  SOCKET _cSocket = INVALID_SOCKET;

  while (true) {
    _cSocket = accept(_socket, (sockaddr*)&clientAddr, &nAddrLend);
    if (_cSocket == INVALID_SOCKET)
      std::cout << "Error Accept to invalid client" << std::endl;

    // 5. send 向客户端发送一条数据
    std::cout << inet_ntoa(clientAddr.sin_addr) << std::endl;
    const char msgBuf[] = "Hello, I'm Server";
    send(_cSocket, msgBuf, strlen(msgBuf) + 1, 0);
  }

  // 6. 关闭 socket
  closesocket(_socket);

  // 清除 Socket 环境
  WSACleanup();
  return 0;
}