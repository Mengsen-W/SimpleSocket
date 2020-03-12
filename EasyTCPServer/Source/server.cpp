// ��ֹͷ�ļ��ĺ궨��ì��
#define WIN32_LEAN_AND_MEAN
// ����ת��������
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <windows.h>

#include <iostream>

// ������ӿ�
// #pragma comment(lib, "ws2_32.lib")

// ʹ�ýṹ�������ݴ��ݲ���
struct DataPackage {
  int age;
  char name[32];
};

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
  // ��IP��ַ
  _sin.sin_addr.S_un.S_addr = INADDR_ANY;  // inet_addr("127.0.0.1") INADDR_ANY
                                           // ���޶�Ĭ��Ϊ�������ж�����
  // ��ȫ��
  if (SOCKET_ERROR == bind(_socket, (sockaddr*)&_sin, sizeof(_sin)))
    std::cout << "Error bind" << std::endl;
  else
    std::cout << "Successed bind" << std::endl;


  // 3. listen ��������˿� ��Ϊ�����ֵĹ���
  if (SOCKET_ERROR == listen(_socket, 5))
    std::cout << "Error listen" << std::endl;
  else
    std::cout << "Successed listen" << std::endl;


  // 4. accept �ȴ����տͻ�������
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

    // 5. �ӿͻ��˽�����Ϣ
    int nLen = recv(_cSocket, _recvBuf, 128, 0);

    // �жϷ���ֵ
    if (nLen <= 0) {
      std::cout << "Client Out" << std::endl;
      break;
    }
    std::cout << "recv: " << _recvBuf << std::endl;

    // 6. ��������
    if (0 == strcmp(_recvBuf, "getName")) {

      // 7. send ��ͻ��˷���һ������
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


  // 8. �ر� socket
  closesocket(_socket);


  // ��� Socket ����
  WSACleanup();
  std::cout << "Mission Finished" << std::endl;
  getchar();
  return 0;
}