// ��ֹͷ�ļ��ĺ궨��ì��
#define WIN32_LEAN_AND_MEAN

#include <WinSock2.h>
#include <windows.h>

// ������ӿ�
#pragma comment(lib, "ws2_32.lib")

int main(void) {
  WORD ver = MAKEWORD(2, 2);
  WSADATA data;

  // ���� Socket
  if (-1 == WSAStartup(ver, &data)) return -1;

  // ֹͣ Socket
  WSACleanup();
  return 0;
}