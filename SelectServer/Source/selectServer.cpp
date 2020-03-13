/*
 * 用 select 网络模型，处理多个客户端
 * 2020年3月13日 13点15分
 * Mengsen_Wang
 */
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <windows.h>

#include <iostream>
#include <vector>

#include "DataBase.h"

#pragma comment(lib, "ws2_32.lib")

std::vector<SOCKET> g_clients;

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
  // 绑定IP地址
  _sin.sin_addr.S_un.S_addr = INADDR_ANY;  // inet_addr("127.0.0.1") INADDR_ANY
                                           // 不限定默认为本机所有都可以
  // 绑定全部
  if (SOCKET_ERROR == bind(_socket, (sockaddr *)&_sin, sizeof(_sin)))
    std::cout << "Error bind" << std::endl;
  else
    std::cout << "Successed bind" << std::endl;

  // 3. listen 监听网络端口 因为由握手的过程
  if (SOCKET_ERROR == listen(_socket, 5))
    std::cout << "Error listen" << std::endl;
  else
    std::cout << "Successed listen" << std::endl;

  while (true) {
    fd_set fdRead;
    fd_set fdWrite;
    fd_set fdExp;

    FD_ZERO(&fdRead);
    FD_ZERO(&fdWrite);
    FD_ZERO(&fdExp);

    FD_SET(_socket, &fdRead);
    FD_SET(_socket, &fdWrite);
    FD_SET(_socket, &fdExp);

    for (size_t n = 0; n < g_clients.size(); ++n) {
      FD_SET(g_clients[n], &fdRead);
    }

    // 第一个参数是指的 fd_set 所有的的描述符(Socket)的范围，而不是数量。
    // 是指的描述符最大值 + 1
    int ret = select(_socket + 1, &fdRead, &fdWrite, &fdExp, nullptr);
    if (ret < 0) {
      std::cout << "Error Select" << std::endl;
      break;
    }

    if (FD_ISSET(_socket, &fdRead)) {
      FD_CLR(_socket, &fdRead);

      // 4. accept 等待接收客户端连接
      sockaddr_in clientAddr = {};
      int nAddrLend = sizeof(clientAddr);
      SOCKET _cSocket = INVALID_SOCKET;

      _cSocket = accept(_socket, (sockaddr *)&clientAddr, &nAddrLend);
      g_clients.push_back(_cSocket);

      if (_cSocket == INVALID_SOCKET)
        std::cout << "Error Accept to invalid client" << std::endl;

      std::cout << "New Client coming in: " << inet_ntoa(clientAddr.sin_addr)
                << "\n"
                << "Port: " << htons(clientAddr.sin_port) << std::endl;
      
    }

    DataHead header = {};
    // 5. 从客户端接收消息，另一端关闭返回0，失败返回-1
    int nLen = recv(_cSocket, (char *)&header, sizeof(DataHead), 0);

    // 判断返回值
    if (nLen <= 0) {
      std::cout << "Client Out" << std::endl;
      break;
    }
    std::cout << "recv Length:" << header.dataLength << "\n" << std::endl;

    // 6. 处理请求
    switch (header.cmd) {
      case CMD_LOGIN: {
        Login login = {};
        recv(_cSocket, (char *)&login + sizeof(DataHead),
             sizeof(Login) - sizeof(DataHead), 0);
        std::cout << "Login UserName = " << login.userName << std::endl;
        std::cout << "Login PassWord = " << login.passWord << std::endl;
        std::cout << "Login Datelength =  " << login.dataLength << std::endl;

        // 判断账号密码是否正确
        // 7. 发送返回
        LoginResult ret;
        send(_cSocket, (const char *)&ret, sizeof(LoginResult), 0);
      } break;

      case CMD_LOGOUT: {
        Logout logout = {};
        recv(_cSocket, (char *)&logout + sizeof(DataHead),
             sizeof(Logout) - sizeof(DataHead), 0);
        std::cout << "Logout UserName = " << logout.userName << std::endl;
        std::cout << "Logout Datelength =  " << logout.dataLength << std::endl;

        // 判断账号密码是否正确
        // 7. 发送返回
        LogoutResult ret;
        send(_cSocket, (const char *)&ret, sizeof(LogoutResult), 0);
      } break;

      default: {
        header.cmd = CMD_ERROR;

        // 处理异常
        header.dataLength = 0;
        send(_cSocket, (const char *)&header, sizeof(DataHead), 0);
      }
    }
  }

  // 8. 关闭 socket
  closesocket(_socket);

  // 清除 Socket 环境
  WSACleanup();
  std::cout << "Mission Finished" << std::endl;

  return 0;
}
