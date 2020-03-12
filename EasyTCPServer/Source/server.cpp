// ��ֹͷ�ļ��ĺ궨��ì��
#define WIN32_LEAN_AND_MEAN
// ����ת��������
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <windows.h>

#include <iostream>

// ������ӿ�
// #pragma comment(lib, "ws2_32.lib")

// ����ö��
enum CMD { CMD_LOGIN, CMD_LOGOUT, CMD_ERROR };

// ��ͷ����
struct DataHead {
  short dataLength;  //  ���ݳ���
  short cmd;         //  ����
};

// ��������
struct Login {
  char userName[32];
  char passWord[32];
};

struct LoginResult {
  int result;
};

struct Logout {
  char userName[32];
};

struct LogoutResult {
  int result;
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
  if (SOCKET_ERROR == bind(_socket, (sockaddr *)&_sin, sizeof(_sin)))
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

  _cSocket = accept(_socket, (sockaddr *)&clientAddr, &nAddrLend);

  if (_cSocket == INVALID_SOCKET)
    std::cout << "Error Accept to invalid client" << std::endl;
  std::cout << "New Client coming in: " << inet_ntoa(clientAddr.sin_addr)
            << "\n"
            << "Port: " << htons(clientAddr.sin_port) << std::endl;

  while (true) {
    DataHead header = {};
    // 5. �ӿͻ��˽�����Ϣ
    int nLen = recv(_cSocket, (char *)&header, sizeof(DataHead), 0);

    // �жϷ���ֵ
    if (nLen <= 0) {
      std::cout << "Client Out" << std::endl;
      break;
    }
    std::cout << "recv: " << header.cmd << "\n"
              << "recv Length:" << header.dataLength << "\n"
              << std::endl;

    // 6. ��������
    switch (header.cmd) {
      case CMD_LOGIN: {
        Login login = {};
        recv(_cSocket, (char *)&login, sizeof(Login), 0);
        std::cout << "Login UserName = " << login.userName << std::endl;
        std::cout << "Login PassWord = " << login.passWord << std::endl;

        // �ж��˺������Ƿ���ȷ
        // 7. ���ͷ���
        LoginResult ret = {0};
        send(_cSocket, (const char *)&header, sizeof(DataHead), 0);
        send(_cSocket, (const char *)&ret, sizeof(LoginResult), 0);
      } break;

      case CMD_LOGOUT: {
        Logout logout = {};
        recv(_cSocket, (char *)&logout, sizeof(Logout), 0);
        std::cout << "Logout UserName = " << logout.userName << std::endl;

        // �ж��˺������Ƿ���ȷ
        // 7. ���ͷ���
        LogoutResult ret = {0};
        send(_cSocket, (const char *)&header, sizeof(DataHead), 0);
        send(_cSocket, (const char *)&ret, sizeof(LogoutResult), 0);
      } break;

      default: {
        header.cmd = CMD_ERROR;

        // �����쳣
        header.dataLength = 0;
        send(_cSocket, (const char *)&header, sizeof(DataHead), 0);
      }
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