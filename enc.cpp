#include "includes.h"

void sendEnc(SOCKET s, string data, string akey, string aiv) //encrypt and send data
{
	string encrypted = security::encrypt(data, akey, aiv);
	send(s, encrypted.c_str(), sizeof(encrypted), 0);
	//printf("sent %s\n", encrypted.c_str()); //for debugging or idk
}
string recvDec(SOCKET s, string akey, string aiv) //receive and decrypt data
{
	char buffer[2048];
	recv(s, buffer, sizeof(buffer), 0);
	string decrypted = security::decrypt(buffer, akey, aiv);
	memset(buffer, 0, sizeof(buffer));
	//printf("received %s\n", decrypted.c_str()); //for debugging or idk
	return decrypted;
}