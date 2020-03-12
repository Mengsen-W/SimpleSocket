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
enum CMD {
  CMD_LOGIN,
  CMD_LOGIN_RESULT,
  CMD_LOGOUT,
  CMD_LOGOUT_RESULT,
  CMD_ERROR
};

// Ϊ��������ʾ��������ǵ������������Գ����ദ��

// ��ͷ����
struct DataHead {
  short dataLength;  //  ���ݳ���
  short cmd;         //  ����
};

// ��������
struct Login : public DataHead {
  Login() {
    dataLength = sizeof(Login);
    cmd = CMD_LOGIN;
  }
  char userName[32];
  char passWord[32];
};

struct LoginResult : public DataHead {
  LoginResult() {
    dataLength = sizeof(DataHead);
    cmd = CMD_LOGIN_RESULT;
    result = 0;
  }
  int result;
};

struct Logout : public DataHead {
  Logout() {
    dataLength = sizeof(DataHead);
    cmd = CMD_LOGOUT;
  }
  char userName[32];
};

struct LogoutResult : public DataHead {
  LogoutResult() {
    dataLength = sizeof(DataHead);
    cmd = CMD_LOGOUT_RESULT;
    result = 0;
  }
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
    std::cout << "recv Length:" << header.dataLength << "\n" << std::endl;

    // 6. ��������
    switch (header.cmd) {
      case CMD_LOGIN: {
        Login login = {};
        recv(_cSocket, (char *)&login + sizeof(DataHead),
             sizeof(Login) - sizeof(DataHead), 0);
        std::cout << "Login UserName = " << login.userName << std::endl;
        std::cout << "Login PassWord = " << login.passWord << std::endl;
        std::cout << "Login Datelength =  " << login.dataLength << std::endl;

        // �ж��˺������Ƿ���ȷ
        // 7. ���ͷ���
        LoginResult ret;
        send(_cSocket, (const char *)&ret, sizeof(LoginResult), 0);
      } break;

      case CMD_LOGOUT: {
        Logout logout = {};
        recv(_cSocket, (char *)&logout + sizeof(DataHead),
             sizeof(Logout) - sizeof(DataHead), 0);
        std::cout << "Logout UserName = " << logout.userName << std::endl;
        std::cout << "Logout Datelength =  " << logout.dataLength << std::endl;

        // �ж��˺������Ƿ���ȷ
        // 7. ���ͷ���
        LogoutResult ret;
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