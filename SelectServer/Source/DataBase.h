#pragma once
// ����ö��
enum CMD {
  CMD_LOGIN,
  CMD_LOGIN_RESULT,
  CMD_LOGOUT,
  CMD_LOGOUT_RESULT,
  CMD_ERROR
};

// Ϊ��������ʾ��������ǵ������������Գ����ദ��

// ��ͷ����
struct DataHead {
  short dataLength;  //  ���ݳ���
  short cmd;         //  ����
};

// ��������
struct Login : public DataHead {
  Login() {
    dataLength = sizeof(Login);
    cmd = CMD_LOGIN;
  }
  char userName[32] = {0};
  char passWord[32] = {0};
};

struct LoginResult : public DataHead {
  LoginResult() {
    dataLength = sizeof(LoginResult);
    cmd = CMD_LOGIN_RESULT;
    result = 0;
  }
  int result;
};

struct Logout : public DataHead {
  Logout() {
    dataLength = sizeof(Logout);
    cmd = CMD_LOGOUT;
  }
  char userName[32] = {0};
};

struct LogoutResult : public DataHead {
  LogoutResult() {
    dataLength = sizeof(LogoutResult);
    cmd = CMD_LOGOUT_RESULT;
    result = 0;
  }
  int result;
};

