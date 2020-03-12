// 防止头文件的宏定义矛盾
#define WIN32_LEAN_AND_MEAN
// 定义转换函数宏
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <windows.h>

#include <iostream>

// 添加链接库
// #pragma comment(lib, "ws2_32.lib")

// 使用结构化的数据传递参数
struct DataPackage {
  int age;
  char name[32];
};

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
  if (SOCKET_ERROR == bind(_socket, (sockaddr*)&_sin, sizeof(_sin)))
    std::cout << "Error bind" << std::endl;
  else
    std::cout << "Successed bind" << std::endl;


  // 3. listen 监听网络端口 因为由握手的过程
  if (SOCKET_ERROR == listen(_socket, 5))
    std::cout << "Error listen" << std::endl;
  else
    std::cout << "Successed listen" << std::endl;


  // 4. accept 等待接收客户端连接
  sockaddr_in clientAddr = {};
  int nAddrLend = sizeof(clientAddr);
  SOCKET _cSocket = INVALID_SOCKET;

  _cSocket = accept(_socket, (sockaddr*)&clientAddr, &nAddrLend);
  if (_cSocket == INVALID_SOCKET)
    std::cout << "Error Accept to invalid client" << std::endl;
  std::cout << "New Client coming in: " << inet_ntoa(clientAddr.sin_addr)
            << std::endl;

  char _recvBuf[128] = {};
  while (true) {

    // 5. 从客户端接收消息
    int nLen = recv(_cSocket, _recvBuf, 128, 0);

    // 判断返回值
    if (nLen <= 0) {
      std::cout << "Client Out" << std::endl;
      break;
    }
    std::cout << "recv: " << _recvBuf << std::endl;

    // 6. 处理请求
    if (0 == strcmp(_recvBuf, "getName")) {

      // 7. send 向客户端发送一条数据
      char msgBuf[] = "I'm Server";
      send(_cSocket, msgBuf, strlen(msgBuf) + 1, 0);

    } else if (0 == strcmp(_recvBuf, "getAge")) {
      char msgBuf[] = "I'm long time";
      send(_cSocket, msgBuf, strlen(msgBuf) + 1, 0);

    } else if (0 == strcmp(_recvBuf, "getInfo")) {
      DataPackage dp = {24, "Mengsen_Wang"};
      send(_cSocket, (const char *)&dp, sizeof(DataPackage), 0);

    }else {
      char msgBuf[] = "getName or getAge or getInfo...";
      send(_cSocket, msgBuf, strlen(msgBuf) + 1, 0);
    }
  }


  // 8. 关闭 socket
  closesocket(_socket);


  // 清除 Socket 环境
  WSACleanup();
  std::cout << "Mission Finished" << std::endl;
  getchar();
  return 0;
}