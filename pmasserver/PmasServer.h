#pragma once
#include <list>
#include <vector>
#include <map>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "socket.h"


using namespace std;

#define RECV_BUFFER_SIZE 20480
#define RECV_TIMEOUT 500

#ifndef WIN32
typedef unsigned long DWORD, *PDWORD, *LPDWORD;
typedef void *LPVOID;
#define WINAPI
#endif
#define PM_INVALID_SOCKET -1

class ConnectionErrorHandler {
public:
	virtual void HandleError(int iErr) = 0;
	virtual void LogError(const char *psz_format, ...) = 0;
	virtual void OnConnectionClosed(void) = 0;
	virtual ~ConnectionErrorHandler() {};
};

class RequestHandler {
public:
	virtual void HandleRequest(vector<string>& Params) = 0;
	virtual ~RequestHandler() {};
};

class TCPConnection {
private:
	int m_ClientSocket;
	char m_pszRecvBuffer[RECV_BUFFER_SIZE];
	map<string,pair<unsigned int,RequestHandler*> > m_HandlersMap;
	bool m_bStopHandle;
	thread* m_HandlerThread;
	friend DWORD WINAPI RequestHandlerThreadFunc( LPVOID lpParam );
	void HandlerLoop(void);
	ConnectionErrorHandler*	m_pErrHandler;
public: 
	TCPConnection(int ClientSocket,ConnectionErrorHandler& ErrHandler) : m_ClientSocket(ClientSocket), m_bStopHandle(false),m_HandlerThread(NULL), m_pErrHandler(&ErrHandler)  {}
	~TCPConnection();
	bool Send(string SentMsg);
	bool SendW(wstring SentMsg);

	bool SendInt(uint32_t uVal);
	bool SendFile(const char* path);
	bool SendBuffer(const char *pBuff,int iSize);
	bool SendBufferW(const wchar_t *pBuff, int iSize);

	bool Receive(string& RecvMsg);
	bool RegisterHandler(string strCmd, unsigned int uNumParams, RequestHandler* Handler);
	bool HandleAll(void);
	void Stop(void);
};

class ConnectionHandler {
public: 
	virtual void HandleData(TCPConnection *pConn) = 0;
	virtual ~ConnectionHandler() {};
};

class TCPServer {
private:
	int m_nPort;
	int m_ListenSocket;
	TCPConnection* m_pConnection;
	static int nActiveServers; 
	thread* m_HandlerThread;
	mutex m_ConnectionMutex;
	condition_variable m_ConnectionCondition;
	ConnectionHandler* m_pConnectionHandler;
	ConnectionErrorHandler*	m_pErrHandler;
	friend DWORD WINAPI ConnectionHandlerThreadFunc( LPVOID lpParam );
	void WaitConnectionInternal(void);

public:
	TCPServer() : m_nPort(0),m_ListenSocket(PM_INVALID_SOCKET),m_pConnection(NULL), m_HandlerThread(NULL), m_pConnectionHandler(NULL), m_pErrHandler(NULL) {}
	virtual ~TCPServer();
	bool Start(int port,ConnectionHandler *pConnHandler, ConnectionErrorHandler *pErrHandler);
	void Stop(void);
	bool WaitConnection(void);
	void ResetConnection(void);
	TCPConnection* getConnection();
	void setConnection(TCPConnection* connection);

};
