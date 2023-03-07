#pragma once

//defines
#define _CRT_NONSTDC_NO_WARNINGS

//main
#include <iostream>
#include <sstream>

//socket
#include <winsock2.h>
#include <ws2tcpip.h>

//crypto
#include "../shared/crypto/crypto.h"

//extra
#pragma comment(lib,"WS2_32")

//namespaces
using namespace std;

/******************************************************/
inline string version = "0.1.0";	//version of your program
inline constexpr int port = 1337;			// port
inline constexpr PCWSTR ip = L"127.0.0.1";	//server ip
/******************************************************/

//declarations
void sendEnc(SOCKET s, string data, string akey, string aiv);
string recvDec(SOCKET s, string akey, string aiv);

string gen_random(const int len, int pid);

bool query_wmic(const string& input, string& out);
bool query();

void program();

inline string m_CPU;
inline string m_ComputerName;
inline string m_Physical;
inline string hwid;