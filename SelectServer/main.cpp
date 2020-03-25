/**
 * Mengsen Wang
 * 2020年3月25日 12点18分
 */

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <signal.h>

#include <cassert>
#include <map>
#include <string>
#include <thread>

#include "Server.h"
#pragma comment(lib, "Ws2_32.lib")

std::map<int, ServerSocket::pointer> players;

void handleRecvData(const ServerSocket::pointer& p, const char* data,
                    int size) {
  // 构造
  std::string info(data, data + size);
  std::cout << "recv: " << info << std::endl;
  for (int i = 0; i < 10; ++i) {
    Sleep(1);
    p->sendMessage("+<html>hello world</html>!");
  }
}

void handleConn(Server* s) {
  try {
    s->waitingForAccept();
  } catch (...) {
    std::cout << "has error \n";
  }
}

std::function<void()> handler;
void signalHandler(int code) {
  std::cout << "handle" << code << std::endl;
  if (handler) {
    handler();
    handler = nullptr;
  }
}

void handleAddNewPlayer(const ServerSocket::pointer& p) {
  std::cout << "hello" << p->id() << std::endl;
  players.erase(p->id());
}

void handleRemovePlayer(const ServerSocket::pointer& p) {
  std::cout << "bye" << p->id() << std::endl;
  players.erase(p->id());
}

ServerSocket::pointer buildClientSocket(const char* addr, u_short port, int id,
                                        HANDLE io) {
  auto connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (connectSocket == INVALID_SOCKET) {
    return nullptr;
  }
  sockaddr_in clientService;
  clientService.sin_family = AF_INET;
  clientService.sin_port = htons(port);
  clientService.sin_addr.s_addr = inet_addr(addr);

  auto iResult =
      connect(connectSocket, (SOCKADDR*)&clientService, sizeof(clientService));
  if (iResult == SOCKET_ERROR) {
    std::cout << "connect function failed with error: " << WSAGetLastError()
              << std::endl;
  }

  iResult = closesocket(connectSocket);
  if (iResult == SOCKET_ERROR) {
    std::cout << "closesocker function failed with error: " << WSAGetLastError()
              << std::endl;
  }
  return nullptr;
}

int main(int argc, char** argv) {
  // 提供中断信号量
  signal(SIGINT, signalHandler);

  // 加载环境
  WSADATA data;
  auto result = WSAStartup(MAKEWORD(2, 2), &data);
  if (result != NO_ERROR) {
    std::cout << "error at WSAStartup\n";
    return 1;
  }

  auto server = std::make_unique<Server>(8901);
  if (!server) {
    WSACleanup();
    return 1;
  }

  server->socketRecv = handleRecvData;
  server->socketClose = handleRemovePlayer;

  bool gameOver = false;
  auto p = server.get();

  // 通过信号停止 Server
  handler = [p] { p->stop(); };

  auto ok = server->startAccept();
  std::unique_ptr<std::thread> t;
  std::unique_ptr<std::thread> io;
  if (ok) {
    std::cout << "ok\n";
    server->newConnect = handleAddNewPlayer;
    t.reset(new std::thread([p] { p->waitingForAccept(); }));
    io.reset(new std::thread([p] { p->waitingForIO(); }));
  }
  std::cout << "continue\n";

  while (server->isRunning()) {
    // in fact this should update main logic
    Sleep(1);
  }

  std::cout << "ok and bye\n";
  gameOver = true;
  if (t && t->joinable()) t->join();
  if (io && io->joinable()) io->join();

  // 此处停止
  players.clear();
  WSACleanup();
  return 0;
}