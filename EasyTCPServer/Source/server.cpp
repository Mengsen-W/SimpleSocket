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

  // ��Socket API ����һ���򵥵� TCP ������
  // 1. ���� socket ����
  SOCKET _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  // 2. bind �����ڽ��տͻ������ӵ���·�˿�

  // sockaddr_in �����ݽṹ�����ڴ�������д
  sockaddr_in _sin = {};
  // ��Э������
  _sin.sin_family = AF_INET;
  // �󶨶˿ں�
  _sin.sin_port = htons(4567);  // host to net unsign short
  // �󶨶˿ں�
  _sin.sin_addr.S_un.S_addr =
      INADDR_ANY;  // inet_addr("127.0.0.1") INADDR_ANY ���޶�Ĭ��
  // ��ȫ��
  if (SOCKET_ERROR == bind(_socket, (sockaddr*)&_sin, sizeof(_sin)))
    std::cout << "Error bind to Netport." << std::endl;

  // 3. listen ��������˿� ��Ϊ�����ֵĹ���
  if (SOCKET_ERROR == listen(_socket, 5))
    std::cout << "Error listen to Netport." << std::endl;

  // 4. accept �ȴ����տͻ�������
  sockaddr_in clientAddr = {};
  int nAddrLend = sizeof(clientAddr);
  SOCKET _cSocket = INVALID_SOCKET;

  while (true) {
    _cSocket = accept(_socket, (sockaddr*)&clientAddr, &nAddrLend);
    if (_cSocket == INVALID_SOCKET)
      std::cout << "Error Accept to invalid client" << std::endl;

    // 5. send ��ͻ��˷���һ������
    std::cout << inet_ntoa(clientAddr.sin_addr) << std::endl;
    const char msgBuf[] = "Hello, I'm Server";
    send(_cSocket, msgBuf, strlen(msgBuf) + 1, 0);
  }

  // 6. �ر� socket
  closesocket(_socket);

  // ��� Socket ����
  WSACleanup();
  return 0;
}