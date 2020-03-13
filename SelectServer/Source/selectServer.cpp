/*
 * �� select ����ģ�ͣ��������ͻ���
 * 2020��3��13�� 13��15��
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

    // ��һ��������ָ�� fd_set ���еĵ�������(Socket)�ķ�Χ��������������
    // ��ָ�����������ֵ + 1
    int ret = select(_socket + 1, &fdRead, &fdWrite, &fdExp, nullptr);
    if (ret < 0) {
      std::cout << "Error Select" << std::endl;
      break;
    }

    if (FD_ISSET(_socket, &fdRead)) {
      FD_CLR(_socket, &fdRead);

      // 4. accept �ȴ����տͻ�������
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
    // 5. �ӿͻ��˽�����Ϣ����һ�˹رշ���0��ʧ�ܷ���-1
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

  return 0;
}
