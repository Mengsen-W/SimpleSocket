/**
 * Mengsen Wang
 * 2020年3月25日 16点35分
 * send 和 recv 数据
 */

#ifndef TY_SERVER_SOCKET_H
#define TY_SERVER_SOCKET_H
#ifndef UNICODE
#define UNICODE
#endif  // !UNICODE
#define WIN32_LEAN_AND_MEAN
#include <mswsock.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

class ServerSocket;

// IOCP 自己的内部状态和通知事件
// 派生后可以拿到自己的信息
struct OverUnit : WSAOVERLAPPED {
  ServerSocket* socket;
  int type;
};

class ServerSocket : public std::enable_shared_from_this<ServerSocket> {
 public:
  using pointer = std::shared_ptr<ServerSocket>;
  typedef std::function<void(const pointer&, const char* data, int size)>
      HandleRecvFunction;
  typedef std::function<void(const pointer&)> HandleClose;
  typedef std::function<void(const pointer&, int, int)> HandleError;

  HandleRecvFunction handleRecv;
  HandleClose handleClose;
  HandleError handleError;

  ServerSocket(int id, SOCKET s);
  ~ServerSocket();

  void onError(int errorType, int errorCode);
  void onClosed();
  void onFinishedSend(int transfered);
  void onFinishedRecv(int transfered);
  int id() const { return m_id; }
  void startRecv();
  void sendMessage(const std::string& info);
  void trySendMore();

 private:
  int m_id;
  SOCKET m_socket;

  using Buffer = std::vector<char>;
  Buffer m_currentRecv;
  std::deque<Buffer> m_recvBuffers;
  std::deque<std::string> m_sendBuffers;

  OverUnit m_recvUnit;
  OverUnit m_sendUnit;
  WSABUF m_sendWSA;
  WSABUF m_recvWSA;

  void sendFrontBuffer();
  std::mutex m_mutex;
};

#endif  // !TY_SERVER_SOCKET_H
