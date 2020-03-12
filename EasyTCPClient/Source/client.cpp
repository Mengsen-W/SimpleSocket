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
  int ret = connect(_sock, (sockaddr *)&_sin, sizeof(sockaddr_in));
  if (SOCKET_ERROR == ret) std::cout << "Disable Connected" << std::endl;

  while (true) {
    std::cout << "Input Message..." << std::endl;

    // ��������
    char cmdBuf[128] = {};
    std::cin >> cmdBuf;
    // 4. ��������
    if (0 == strcmp(cmdBuf, "exit")) {
      break;

    } else if (0 == strcmp(cmdBuf, "login")) {
      // 5. ��������

      Login login ;
      strcpy_s(login.userName, "mengsen");
      strcpy_s(login.passWord, "qaz123~");
      send(_sock, (const char *)&login, sizeof(Login), 0);

      // 6. ���շ�������
      LoginResult res = {};
      recv(_sock, (char *)&res, sizeof(LoginResult), 0);
      std::cout << "LoginResult: " << res.result << std::endl;

    } else if (0 == strcmp(cmdBuf, "logout")) {
      // 5.��������
      Logout logout;
      strcpy_s(logout.userName, "mengsen");
      send(_sock, (const char *)&logout, sizeof(Logout), 0);
      // 6. ���շ�������
      LogoutResult res = {};
      recv(_sock, (char *)&res, sizeof(LogoutResult), 0);
      std::cout << "LogoutResult: " << res.result << std::endl;


    } else
      std::cout << "Error Input: login ? or logout ? "
                << "\n"
                << std::endl;
  }
  // 7. �ر� socket
  closesocket(_sock);

  // ��� Socket ����
  WSACleanup();
  std::cout << "Mission Finished" << std::endl;
  return 0;
}
