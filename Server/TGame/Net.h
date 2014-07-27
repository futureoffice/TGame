#ifndef _NET_H
#define _NET_H
#include "Buffer.h"

class Net
{
	enum NetKind
	{
		SERVER=1,
		ACCEPT,
		CLIENT,
		UNKNOW,
	};
public:
	typedef int OnReceive(Net* net, const char* data, unsigned int len);
	typedef int OnClose(Net* net, const char* msg);
	typedef void OnAccept(Net* net);

	OnReceive* onReceive;
	OnClose* onClose;
	OnAccept* onAccept;

	Net(void);
	bool IsClosed();
	void Server(int, OnAccept, OnClose);
	void Accept(int);
	bool IsInvalid();
	bool IsServer();
	bool IsClient();
	bool IsAccpet();
	int	GetSocket();
	int Recv();
	int Send();
	int Close();
	void SetOnReceive(OnReceive onReceive, OnClose onClose);

	virtual ~Net(void);
private:
	int mSocket;
	NetKind mKind;
	bool mClosed;
	Buffer* mBuffer;
	
};
#endif
