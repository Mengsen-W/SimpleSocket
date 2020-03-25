/**
 * Mengsen Wang
 * 2020年3月25日 11点19分
 */

#define _WINSOCK_DEPRECATED_NO_WARNINGS 
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
  // 监听接收客户端和处理数据发送IO分开
  m_completePort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
  if (!m_completePort) return false;

  m_ioCompletePort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
  if (!m_ioCompletePort) return false;

  m_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (m_listenSocket == INVALID_SOCKET) return false;

  // 绑定IOCP
  CreateIoCompletionPort((HANDLE)m_listenSocket, m_completePort, 0, 0);

  // 绑定本机IP
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

  // 开始监听
  auto result = listen(m_listenSocket, 100);
  if (result == SOCKET_ERROR) {
    std::cout << "listen failed with error: " << WSAGetLastError() << "\n";
    return false;
  }

  // 将函数指针放入 WASarts 加载的 dll 里面
  // 得到链接进入客户端的信息--UID
  DWORD dwBytes = 0;
  GUID guidAcceptEx = WSAID_ACCEPTEX;
  result = WSAIoctl(m_listenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
                    &guidAcceptEx, sizeof(guidAcceptEx), &lpfnAcceptEx,
                    sizeof(lpfnAcceptEx), &dwBytes, nullptr, nullptr);
  if (result == SOCKET_ERROR) {
    std::cout << "WSAIoctl failed with error: " << WSAGetLastError() << '\n';
    return false;
  }

  // 接收客户端socket
  m_currentAcceptSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (m_currentAcceptSocket == INVALID_SOCKET) return false;

  int outBufLen = 1024;
  memset(&m_acceptUnite, 0, sizeof(m_acceptUnite));
  auto ret = lpfnAcceptEx(m_listenSocket, m_currentAcceptSocket,
                          m_acceptBuffer.data(), 0, sizeof(sockaddr_in) + 16,
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

void Server::waitingForAccept() {
  int id = 1;
  while (isRunning()) {
    DWORD bytes = 0;
    ULONG_PTR dwCompletionKey;
    LPOVERLAPPED lpOverlapped = nullptr;
    // 得到完成的事件
    auto ok = GetQueuedCompletionStatus(m_completePort, &bytes,
                                        &dwCompletionKey, &lpOverlapped, 1000);
    if (ok) {
      std::string info(m_acceptBuffer.begin(), m_acceptBuffer.begin() + bytes);

      // 把已经连接进入的socket放入处理IOCP中
      CreateIoCompletionPort((HANDLE)m_currentAcceptSocket, m_ioCompletePort, 0,
                             0);

      // 新客户端进入交给ServerSocket处理
      auto ns = std::make_shared<ServerSocket>(id, m_currentAcceptSocket);

      // 导入回调事件
      ns->handleClose = socketClose;
      ns->handleError = socketError;
      ns->handleRecv = socketRecv;
      if (newConnect) newConnect(ns);

      ++id;

      // 重置
      m_currentAcceptSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      ok = tryNewConn();
      if (!ok) break;
    } else {
      auto errorCode = GetLastError();
      if (errorCode == WAIT_TIMEOUT) continue;
      std::cout << "has error" << errorCode << " in accept new client"
                << WSAGetLastError() << '\n';
      break;
    }
  }
}

void Server::waitingForIO() {
  while (m_running) {
    DWORD bytes = 0;
    ULONG_PTR dwCompletionKey;
    LPOVERLAPPED lpOverlapped = nullptr;
    auto ok = GetQueuedCompletionStatus(m_ioCompletePort, &bytes,
                                        &dwCompletionKey, &lpOverlapped, 1000);
    if (ok) {
      auto unit = (OverUnit*)lpOverlapped;
      assert(unit);
      auto socket = unit->socket;
      if (bytes == 0) {
        socket->onClosed();
      } else {
        if (unit->type == 0) {
          socket->onFinishedRecv(bytes);
        } else if (unit->type == 1) {
          socket->onFinishedSend(bytes);
          socket->trySendMore();
        }
      }
    } else {
      // 错误处理
      auto errorCode = GetLastError();
      if (errorCode == WAIT_TIMEOUT) continue;
      std::cout << "has error" << errorCode << " in accept new client"
                << WSAGetLastError() << '\n';
      if (!lpOverlapped)
        break;
      else {
        OverUnit* unit = (OverUnit*)lpOverlapped;
        auto socket = unit->socket;
        if (socket) {
          socket->onError(unit->type, WSAGetLastError());
        }
      }
    }
  }
}
