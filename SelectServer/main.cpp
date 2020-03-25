/**
 * Mengsen Wang
 * 2020年3月25日 12点18分
 */

#include <signal.h>

#include <cassert>
#include <map>
#include <string>
#include <thread>

#include "Sercer.h"
#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char** argv) {
  singal(SIGINT, signalHandler);
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
  server->socketClose = handlrRemovePlayer;

  auto clientIO = CreatIOComplettionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
  bool gameOver = false;
  std::thread updateClientIO(
      [clientIO, &gameOver]() { updataClientIO(clientIO, gameOver); });

  auto p = server.get();
  handler = [p] { p->stop(); };

  auto ok = server->startAccept();
  std::unique_ptr<std::thread> t;
  std::unique_ptr<std::thread> io;
  if (ok) {
    std::cout << "ok\n";
    server->newConn = handleAddNewPlayer;
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
  updateClientIO.join();
  if (t && t->joinable()) t->join();
  if (io && io->joinable()) io->join();

  CloseHandle(clientiIO);
  players.clear();
  WSACleanup();
  return 0;
}