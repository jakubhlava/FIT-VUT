#ifndef client
#define client

#include <unistd.h>
#include <getopt.h>
#include <string>
#include <iostream>
#include <vector>
#include "base64.hpp"
#include <sys/socket.h>
#include <regex>
#include <netdb.h>
#include <arpa/inet.h>
#include <filesystem>
#include <fstream> 

using namespace std;
namespace fs = std::filesystem;

void print_help();
int cmd_register(string username, string password);
int cmd_login(string username, string password);
int cmd_list();
int cmd_send(string recipient, string subject, string body);
int cmd_fetch(int message_id);
int cmd_logout();

int process_command(vector<string> command);
string process_address(string input);
int write_token(string login_token);
string read_token();
void destroy_token();
string trimquotes(string input);
string cmdsend(string command);

#define STATUS_OK       0
#define STATUS_ERR      1
#define STATUS_PARAMS   2
#define STATUS_CONN     3

#define RECV_BUF_SIZE   16384

const string tokenpath = "./login-token";
const fs::path tokenfile = fs::path(tokenpath);

#endif