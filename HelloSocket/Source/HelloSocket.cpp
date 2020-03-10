// 防止头文件的宏定义矛盾
#define WIN32_LEAN_AND_MEAN

#include <WinSock2.h>
#include <windows.h>

// 添加链接库
#pragma comment(lib, "ws2_32.lib")

int main(void) {
  WORD ver = MAKEWORD(2, 2);
  WSADATA data;

  // 启动 Socket
  if (-1 == WSAStartup(ver, &data)) return -1;

  // 停止 Socket
  WSACleanup();
  return 0;
}