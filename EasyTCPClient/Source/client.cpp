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

  // 用Socket API 建立一个简单的 TCP 客户端

  // 1. 建立一个 socket
  SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
  if (INVALID_SOCKET == _sock)
    std::cout << "unable to establish Socket" << std::endl;

  // 2. 连接服务器 connect
  sockaddr_in _sin = {};
  _sin.sin_family = AF_INET;
  _sin.sin_port = htons(4567);
  _sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
  int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
  if (SOCKET_ERROR == ret) std::cout << "Disable Connected" << std::endl;

  while (true) {
    // 输入请求
    char cmdBuf[128] = {};
    std::cin >> cmdBuf;
    // 4. 处理请求
    if (0 == strcmp(cmdBuf, "exit")) {
      break;
    } else {
      // 5. 发送请求
      send(_sock, cmdBuf, strlen(cmdBuf) + 1, 0);
    }
    // 6. 接收服务器信息 recv
    char recvBuf[128] = {};
    int nken = recv(_sock, recvBuf, 128, 0);
    if (nken > 0) {
      std::cout << "Successed Recv: "

                << recvBuf << std::endl;
    }
  }

  // 4. 关闭 socket
  closesocket(_sock);

  // 清除 Socket 环境
  WSACleanup();
  std::cout << "Mission Finished" << std::endl;
  getchar();
  return 0;
}
