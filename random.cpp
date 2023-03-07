#include "includes.h"

string gen_random(const int len, int pid)
{
	string temp;
	static const char chars[] = "0123456789" "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	srand(((unsigned)time(NULL) * pid) / 2);
	for (int i = 0; i < len; ++i)
		temp += chars[rand() % (sizeof(chars) - 1)];
	return temp;
}