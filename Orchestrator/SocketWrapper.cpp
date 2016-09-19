#include "SocketWrapper.h"

int SocketWrapper::count = 0;

WSADATA wsaData;
int WSAStartupResult;
bool IsInitialized;
fd_set master;
fd_set read_fds;
int fdmax;

SocketWrapper::SocketWrapper(PCSTR host, PCSTR port)
{
	Socket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;

	if (!IsInitialized)
	{
		Initialize();
	}

	CreateSocket(host, port);

	count++;
}

SocketWrapper::~SocketWrapper()
{
	CloseSocket();
	count--;
	if (count == 0)
	{
		WSACleanup();
		IsInitialized = false;
	}
}

bool SocketWrapper::Initialize()
{
	WSAStartupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (WSAStartupResult != 0) {
		printf("WSAStartup failed: %d\n", WSAStartupResult);
		return false;
	}
	IsInitialized = true;
	return true;
}

bool SocketWrapper::CreateSocket(PCSTR host, PCSTR port)
{
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	int iResult = getaddrinfo(host, port, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return false;
	}

	// Create a SOCKET for connecting to server
	Socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	// If iMode!=0, non-blocking mode is enabled.
	u_long iMode = 1;
	ioctlsocket(Socket, FIONBIO, &iMode);

	if (Socket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return false;
	}

	// Setup the TCP listening socket
	iResult = bind(Socket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(Socket);
		WSACleanup();
		return false;
	}

	freeaddrinfo(result);

	if (listen(Socket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(Socket);
		WSACleanup();
		return false;
	}

	return true;
}

bool SocketWrapper::SendData(char *Buffer)
{
	if (ClientSocket == INVALID_SOCKET)
	{
		return false;
	}

	int iResult = send(ClientSocket, Buffer, (int)strlen(Buffer), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(Socket);
		WSACleanup();
		return false;
	}

	printf("Bytes Sent: %ld\n", iResult);
	return true;

}

int SocketWrapper::ReceiveData(char (&Buffer)[DEFAULT_BUFLEN])
{
	
	// Accept a client socket
	SOCKET TempSocket = accept(Socket, NULL, NULL);
	if (TempSocket != INVALID_SOCKET) {
		ClientSocket = TempSocket;
		printf("Socket connection!!");
	}

	if (ClientSocket == INVALID_SOCKET)
	{
		return 0;
	}

	int Nbytes = recv(ClientSocket, Buffer, DEFAULT_BUFLEN, 0);

	if (Nbytes > 0)
	{
		memcpy(Buffer + Nbytes, "\0", 1);
	}

	return Nbytes;
}

bool SocketWrapper::CloseSocket()
{
	return !!closesocket(Socket);
}