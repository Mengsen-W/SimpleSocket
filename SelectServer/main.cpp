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


  WSACleanup();
  return 0;
}