/**
 * Mengsen Wang
 * 2020年3月25日 11点19分
 */

#include "Server.h"

#include <cassert>
#include <string>

Server::Server(u_short p)
    : m_port(p),
      m_listenSocket(INVALID_SOCKET),
      lpfnAcceptEx(nullptr),
      m_currentAcceptSocket(INVALID_SOCKET),
      m_ioCompletePort(NULL),
      m_running(false),
      m_acceptBuffer(1024) {}

Server::~Server() {
  if (m_listenSocket != INVALID_SOCKET) closesocket(m_listenSocket);
  CloseHandle(m_ioCompletePort);
  CloseHandle(m_completePort);
}

bool Server::startAccept() {
  m_completePort = CreatIOCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
  if (!m_completePort) return false;

  m_ioCompletePort = CreatIOCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
  if (!m_completePort) return false;

  m_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (m_listenSocket == INVALID_SOCKET) return false;

  CreateIoCompletionPort((HANDLE)m_listenSocket, m_completePort, 0, 0);

  hostent* localHost = gethostbyname("");
  char ip[64];
  inet_ntop(AF_INET, (struct in_addr*)*localHost->h_addr_list, ip, sizeof(ip));
  sockaddr_in service;
  service.sin_family = AF_INET;
  service.sin_port = htons(m_port);
  service.sin_addr.s_addr = inet_addr(ip);

  BOOL reuse = TRUE;
  setsockopt(m_listenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse,
             sizeof(reuse));
  if (bind(m_listenSocket, (SOCKADDR*)&service, sizeof(service)) ==
      SOCKET_ERROR) {
    std::cout << "bind failed with error: " << WSAGetLastError() << '\n';
    return false;
  }

  DWORD dwBytes = 0;
  GUID guidAcceptEx = WSAID_ACCEPTEX;
  int result = WSAIoctl(m_listenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
                        &guidAcceptEx, sizeof(guidAcceptEx), &lpfnAcceptEx,
                        sizeof(lpfnAcceptEx), &dwBytes, nullptr, nullptr);
  if (result == SOCKET_ERROR) {
    std::cout << "WSAIoctl failed with error: " << WSAGetLastError() << '\n';
    return false;
  }

  m_currentAcceptSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (m_currentAcceptSocket == INVALID_SOCKET) return false;

  int outBufLen = 1024;
  memset(&m_acceptUnite, 0, sizeof(m_acceptUnite));
  auto ret =
      lpfnAcceptEx(m_listenSocket, m_currentAcceptSocket, m_acceptBuffer.data(),
                   0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
                   sizeof(sockaddr_in) + 16, nullptr, &m_acceptUnite);
  if (ret == FALSE && WSAGetLastError() == ERROR_IO_PENDING) {
    std::cout << "start listen" << ip << " : " << m_port << std::endl;
    m_running = true;
  }
  return m_running;
}

bool Server::tryNewConn() {
  // char lpOutPutBuf[1024]
  int outBufLen = 1024;
  memset(&m_acceptUnite, 0, sizeof(WSAOVERLAPPED));
  auto ret = lpfnAcceptEx(m_listenSocket, m_currentAcceptSocket,
                          m_acceptBuffer.data(), 0,
                          // outBufLen - (sizeof(sockaddr_in) + 16 * 2
                          sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
                          nullptr, &m_acceptUnite);
  if (ret == FALSE && WSAGetLastError() == ERROR_IO_PENDING) return true;
}
