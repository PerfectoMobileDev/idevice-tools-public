// InstrumentsServer.cpp
#ifdef WIN32
#include "stdafx.h"
#endif
#include "PmasServer.h"
#ifdef WIN32
#include <WS2tcpip.h>
#endif
#include <sstream>
#include <sys/stat.h>
#include <future>
#ifndef WIN32
#include <arpa/inet.h>
#endif
#include <errno.h>

using Ms = std::chrono::milliseconds;

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#pragma comment(lib, "Ws2_32.lib")
#else
#define _stat stat
#define _fstat fstat
#endif

#define SAFE_DELETE(X) {if(X!=NULL) {delete X; X = NULL;}}

// TCPServer Methods
// =================

int TCPServer::nActiveServers = 0;
typedef pair<RequestHandler*, vector<string>*> THandlerThreadParams;

bool TCPServer::Start(int port,ConnectionHandler *pConnHandler, ConnectionErrorHandler *pErrHandler) {
//	int iResult;

	m_nPort = port;
	m_pConnectionHandler = pConnHandler;
	m_pErrHandler = pErrHandler;

	m_ListenSocket = socket_create(m_nPort);

	return true;
}

void TCPServer::Stop(void) {
	if(nActiveServers) {
		nActiveServers--;
		// Free the resources
		if(m_ListenSocket != PM_INVALID_SOCKET)
			socket_close(m_ListenSocket);
		// clear connections
		SAFE_DELETE(m_pConnection);
		// See that wait operations finish (wait up to 10 seconds)
		unique_lock<mutex> lk(m_ConnectionMutex);
		m_ConnectionCondition.wait_for(lk, Ms(10000));
		// cleanup
		if(!nActiveServers) {
#ifdef WIN32
			WSACleanup();
#endif
		}
	}
}

DWORD WINAPI ConnectionHandlerThreadFunc( LPVOID lpParam ) {
	((TCPServer*)lpParam)->WaitConnectionInternal();
    return 0;
}

bool TCPServer::WaitConnection(void) {
	m_HandlerThread = new thread(ConnectionHandlerThreadFunc, this);
    return (m_HandlerThread != NULL);
}

void TCPServer::WaitConnectionInternal(void) {
	int ClientSocket = PM_INVALID_SOCKET;
	m_ConnectionMutex.lock();

	SAFE_DELETE(m_pConnection);
	m_pConnection = NULL;

	// Accept a client socket
	ClientSocket = socket_accept(m_ListenSocket, m_nPort);

	if (ClientSocket == PM_INVALID_SOCKET)
	{
		int iErr = errno;
		if(m_pErrHandler)
			m_pErrHandler->LogError("TCPServer::Start - accept failed: %d", iErr);
		socket_close(m_ListenSocket);
		if(m_pErrHandler)
			m_pErrHandler->HandleError(iErr);
		m_ConnectionMutex.unlock();
		m_ConnectionCondition.notify_all();
		return;
	}

	TCPConnection *pConn = new TCPConnection(ClientSocket,*m_pErrHandler);
	m_pConnection = pConn;
	m_ConnectionMutex.unlock();
	m_ConnectionCondition.notify_all();

	if(m_pConnectionHandler)
		m_pConnectionHandler->HandleData(pConn);

}

void TCPServer::ResetConnection(void) {
	SAFE_DELETE(m_pConnection);
}

TCPServer::~TCPServer() {
	if(m_pErrHandler)
		m_pErrHandler->LogError("Destroying TCPServer...");
	Stop();
	if(m_pErrHandler)
		m_pErrHandler->LogError("TCPServer destroyed.");
	// clear handlers
	SAFE_DELETE(m_pConnectionHandler);
	SAFE_DELETE(m_pErrHandler);
}

// TCPConnection Methods
// =====================


bool TCPConnection::Send(string SentMsg) {
	return SendBuffer(SentMsg.c_str(), SentMsg.length());
}


bool TCPConnection::SendW(wstring SentMsgW) {
	return SendBufferW(SentMsgW.c_str(), SentMsgW.length());
}


bool TCPConnection::SendInt(uint32_t uVal) {
	uint32_t ValToSend = htonl(uVal);
	return SendBuffer((char*)&ValToSend,sizeof(uint32_t));
}

bool TCPConnection::SendBufferW(const wchar_t *pBuff, int iSize)
{
	bool bRetVal = true;
	int iSendResult = socket_send(m_ClientSocket, (void*)pBuff, iSize*sizeof(wchar_t)); //send(m_ClientSocket, (const char*)pBuff, iSize*sizeof(wchar_t), 0);

	if (iSendResult < 0)
	{
		m_pErrHandler->LogError("send failed: %d", errno);
		bRetVal = false;
	}
	else {
		//m_pErrHandler->LogError("Bytes sent: %ld", iSendResult);
	}
	return bRetVal;
}


bool TCPConnection::SendBuffer(const char *pBuff,int iSize) 
{
	bool bRetVal = true;
	int iSendResult = socket_send(m_ClientSocket, (void*)pBuff, iSize);

	if (iSendResult < 0)
	{
		m_pErrHandler->LogError("send failed: %d", errno);
		bRetVal = false;
	}
	else {
		//m_pErrHandler->LogError("Bytes sent: %ld", iSendResult);
	}
	return bRetVal;
}

#define BUF_SIZE 1024
bool TCPConnection::SendFile(const char *path) {
	bool bRetVal = true;
	FILE* fp = fopen(path, "rb");
	if (!fp)
		return false;
	// get file size
	int fn = fileno(fp);
	struct _stat st;
	_fstat(fn, &st);
	m_pErrHandler->LogError("About to send a file of %ld bytes", st.st_size);
	// send size
	int iSendResult = socket_send(m_ClientSocket, (void*)&st.st_size, 4);
	
	char file_buff[BUF_SIZE];
	size_t read_size=0;
	int bytes_sent = 0;
	do {
		bytes_sent += read_size;
		read_size = fread(file_buff, 1, BUF_SIZE, fp);
		if (!read_size)
			break;
		//file_buff[read_size - 1] = 1;
		iSendResult = socket_send(m_ClientSocket, (void*)file_buff, read_size);
		m_pErrHandler->LogError("Sending file - sent %ld bytes", iSendResult);
	} while (iSendResult >=0);
	fclose(fp);
	if (iSendResult < 0)
	{
		m_pErrHandler->LogError("send failed: %d", errno);
		bRetVal = false;
	}
	else {
		m_pErrHandler->LogError("File sent - %ld bytes", bytes_sent);
	}

	return bRetVal;
}

bool TCPConnection::Receive(string& RecvMsg) {
	bool bRetVal = false;
	int iResult = socket_receive_timeout(m_ClientSocket, m_pszRecvBuffer, RECV_BUFFER_SIZE, 0, RECV_TIMEOUT);
    //m_pErrHandler->LogError("Received: %s", m_pszRecvBuffer);
	if (iResult > 0)
	{
		RecvMsg = m_pszRecvBuffer;
		bRetVal =  true;	
	}
	else {
		if(iResult == 0) {
			m_pErrHandler->LogError("Recv: Connection closed");
			m_pErrHandler->OnConnectionClosed();
		}
		else
			m_pErrHandler->LogError("recv failed: %d", errno);
	}
	return bRetVal;
}

bool TCPConnection::RegisterHandler(string strCmd, unsigned int uNumParams, RequestHandler* Handler) {
	if(m_HandlersMap.size() && (m_HandlersMap.find(strCmd) != m_HandlersMap.end()))
		return false;

	m_HandlersMap[strCmd] = make_pair(uNumParams, Handler);
	return true;
}

DWORD WINAPI HandlerThreadFunction(LPVOID lpParam) {
	THandlerThreadParams* threadParams = ((THandlerThreadParams*)lpParam);
	std::vector<string>* pParamsForThread = threadParams->second;
	threadParams->first->HandleRequest(*pParamsForThread);
	delete pParamsForThread;
	delete threadParams;
	return 0;
}
void TCPConnection::HandlerLoop(void) { 
	mutex criticalSection;
	// DEBUG
	int BuffIndex = 0;
	unsigned int ParamsExpected = 0;
	unsigned int unlimited = 1000;
	RequestHandler *pRequestHandlerTmp = NULL;
	vector<string> Params;
	while (!m_bStopHandle) {
		string strRcv;
		int iRecvRes = socket_receive_timeout(m_ClientSocket, m_pszRecvBuffer + BuffIndex, 1, 0, RECV_TIMEOUT); // get command from np_hs
		if (iRecvRes > 0) {
			BuffIndex += iRecvRes;
			if (BuffIndex == RECV_BUFFER_SIZE)
			{
				if (m_pszRecvBuffer[BuffIndex - 1] != ':')
				{
					pRequestHandlerTmp = NULL;
					m_pErrHandler->LogError("Recv: Error - buffer is full!!!");
					BuffIndex = 0;
				}
			}
			if (m_pszRecvBuffer[BuffIndex - 1] == ':') {
				m_pszRecvBuffer[BuffIndex - 1] = '\0';
				strRcv = m_pszRecvBuffer;
				if (!pRequestHandlerTmp) {// Identify command name
					map<string, pair<unsigned int, RequestHandler*>>::iterator iter = m_HandlersMap.find(strRcv);
					if (iter != m_HandlersMap.end()) {
						pRequestHandlerTmp = iter->second.second;
						ParamsExpected = iter->second.first;
					}
					else
						m_pErrHandler->LogError("Received unknown command: %s", m_pszRecvBuffer);
				}
				else { // store params
					if (ParamsExpected) {
						Params.push_back(strRcv);
						if (ParamsExpected == unlimited) { // In case that the PMAS command expected unlimited params, it will stop read when getting an empty string param.
							if (strRcv.empty()) {
								ParamsExpected = 0;
							}
						} else {
							ParamsExpected--;
						}
					}
				}
				BuffIndex = 0;
				// call handler if necessary
				if (pRequestHandlerTmp&&!ParamsExpected) {
					criticalSection.lock();
					THandlerThreadParams* threadParams = new(THandlerThreadParams);
					vector<string>* ParamsForThread = new(vector<string>)(Params);
					threadParams->first = pRequestHandlerTmp;
					threadParams->second = ParamsForThread;

					thread new_thread;
					try {
						new_thread = thread(HandlerThreadFunction, threadParams);
					} catch (exception &e) {
						int iErr = errno;
						m_pErrHandler->LogError("TCPServer::HandlerLoop - cannot create new thread: %d", iErr);
						return;
					}
					new_thread.detach();

					Params.clear();
					pRequestHandlerTmp = NULL;
					ParamsExpected = 0;
					criticalSection.unlock();
				}
			}
		}
		else {
			if(iRecvRes != 0)
			{
				if(iRecvRes == -EAGAIN)
				{
					m_pErrHandler->LogError("Recv: Connection closed");
				}
				else
				{
					int iErr = errno;
					m_pErrHandler->LogError("Recv: Unknown error (Result: %d, Error: %d) - Closing connection", iRecvRes, iErr);
					m_pErrHandler->HandleError(iErr);

				}
				m_pErrHandler->OnConnectionClosed();
				break;
			}
			/*if (iRecvRes == 0)
			{
				// Nothing to read
				continue;
			}*/
		}
	}
	Params.clear();
	m_HandlerThread = NULL;
}
DWORD WINAPI RequestHandlerThreadFunc( LPVOID lpParam ) 
{
	((TCPConnection*)lpParam)->HandlerLoop();
    return 0; 
} 

bool TCPConnection::HandleAll(void) {
	// check if handler already running
	if(m_HandlerThread != NULL)
		return false;
	// set socket options
	//DWORD dwRecvTimeout = RECV_TIMEOUT;
	//if(setsockopt(m_ClientSocket,SOL_SOCKET,SO_RCVTIMEO,(char*)&dwRecvTimeout,sizeof(DWORD)) < 0)
	//	m_pErrHandler->LogError("Failed setting receive timeout");

	// ----------------------------------------------------------------------
	// MICKEL: Set LINGER options 
	// ----------------------------------------------------------------------
	/*linger lin;
	lin.l_onoff = 1;
	lin.l_linger = 5;
	if (setsockopt(m_ClientSocket, SOL_SOCKET, SO_LINGER, (const char*)(&lin), sizeof(lin)) == -1)
		perror("setsockopt() SO_LINGER");*/
	// ----------------------------------------------------------------------

	// create thread
	m_bStopHandle = false;
	m_HandlerThread = new thread (RequestHandlerThreadFunc, this);
    return (m_HandlerThread != NULL);
}

void TCPConnection::Stop(void) {
	m_bStopHandle = true;
	if(m_HandlerThread) {
		auto future = async(launch::async, &thread::join, m_HandlerThread);
		future.wait_for(chrono::seconds(60));
	}
	m_HandlerThread = NULL;
}

TCPConnection::~TCPConnection() {
	m_pErrHandler->LogError("Destroying TCPConnection...");
	Stop();
	socket_close(m_ClientSocket);
	m_pErrHandler->LogError("TCPConnectionDestroyed");
	// clear handlers
	for (map<string, pair<unsigned int, RequestHandler*>>::iterator iter = m_HandlersMap.begin(); iter != m_HandlersMap.end(); iter++)
		if (iter->second.second) {
			delete iter->second.second;
			iter->second.second = NULL;
		}
}

TCPConnection* TCPServer::getConnection() { return m_pConnection; }
void TCPServer::setConnection(TCPConnection* connection) { m_pConnection = connection; }
