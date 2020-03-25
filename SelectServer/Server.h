/**
 * Mengsen Wang
 * 2020年3月25日 11点11分
 */

#ifndef YT_WIN_IOCP_SERVER_H
#define YT_WIN_IOCP_SERVER_H
#ifndef UNICODE
#define UNICODE
#endif  // UNICODE
#define WIN32_LEAN_AND_MEAN

#include <Winsock2.h>
#include <ws2tcpip.h>
#include <mswSock.h>

#include <cstdio>
#include <functional>
#include <iostream>

#include "ServerSocket.h"
#pragma comment(lib, "Ws2_32.lib")

class Server {
 public:
  Server(u_short port);
  ~Server();
  bool startAccept();
  void waitingForAccept();
  void waitingForIO();
  bool isRunning() const { return m_running; }
  void stop() { m_running = false; }
  typedef std::function<void(ServerSocket::pointer)> HandleNewConnect;

  // 供使用者回调函数
  HandleNewConnect newConnect;

  ServerSocket::HandleRecvFunction socketRecv;
  ServerSocket::HandleClose socketClose;
  ServerSocket::HandleError socketError;

 private:
  u_short m_port;
  SOCKET m_listenSocket;
  HANDLE m_completePort;
  LPFN_ACCEPTEX lpfnAcceptEx = NULL;
  SOCKET m_currentAcceptSocket;
  WSAOVERLAPPED m_acceptUnite;
  HANDLE m_ioCompletePort;
  bool m_running;
  std::vector<char> m_acceptBuffer;
  bool tryNewConn();
};
#endif  //  YT_WIN_IOCP_SERVER_N
