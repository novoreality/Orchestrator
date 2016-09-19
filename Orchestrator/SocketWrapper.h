#pragma once

#include "VrDefines.h"

class SocketWrapper
{
public:

	SocketWrapper();
	SocketWrapper(PCSTR host, PCSTR port);
	~SocketWrapper();
	bool SendData(char *Buffer);
	int ReceiveData(char (&Buffer)[DEFAULT_BUFLEN]);

private:

	static bool Initialize();
	bool CreateSocket(PCSTR host, PCSTR port);
	bool CloseSocket();


	struct addrinfo *result;
	struct addrinfo *ptr;
	struct addrinfo hints;
	SOCKET Socket;
	SOCKET ClientSocket;
	static int count;


};
