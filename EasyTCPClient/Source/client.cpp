// ��ֹͷ�ļ��ĺ궨��ì��
#define WIN32_LEAN_AND_MEAN
// ����ת��������
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <windows.h>

#include <iostream>

// ������ӿ�
// #pragma comment(lib, "ws2_32.lib")

int main(void) {
  WORD ver = MAKEWORD(2, 2);
  WSADATA data;

  // ���� Socket
  if (-1 == WSAStartup(ver, &data)) {
    std::cout << "Error Startup" << std::endl;
    return -1;
  }

  // ---------------------

  // ��Socket API ����һ���򵥵� TCP �ͻ���

  // 1. ����һ�� socket
  SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
  if (INVALID_SOCKET == _sock)
    std::cout << "unable to establish Socket" << std::endl;

  // 2. ���ӷ����� connect
  sockaddr_in _sin = {};
  _sin.sin_family = AF_INET;
  _sin.sin_port = htons(4567);
  _sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
  int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
  if (SOCKET_ERROR == ret) std::cout << "Disable Connected" << std::endl;

  // 3. ���շ�������Ϣ recv
  char recvBuf[256] = {};
  int nken = recv(_sock, recvBuf, 256, 0);
  if (nken > 0) {
    std::cout << "Successed Recv"
              << "\n"
              << recvBuf << std::endl;
  }
  // 4. �ر� socket
  closesocket(_sock);

  // ��� Socket ����
  WSACleanup();
  getchar();
  return 0;
}
