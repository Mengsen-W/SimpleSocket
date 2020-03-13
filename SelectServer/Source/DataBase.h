#pragma once
// 命令枚举
enum CMD {
  CMD_LOGIN,
  CMD_LOGIN_RESULT,
  CMD_LOGOUT,
  CMD_LOGOUT_RESULT,
  CMD_ERROR
};

// 为了网络演示，如果考虑到生产环境可以抽象类处理

// 包头数据
struct DataHead {
  short dataLength;  //  数据长度
  short cmd;         //  命令
};

// 包体数据
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

