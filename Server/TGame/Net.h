#ifndef _NET_H
#define _NET_H

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
	Net(void);
	bool IsClosed();
	void Server(int);
	void Accpet(int);
	bool IsInvalid();
	bool IsServer();
	bool IsClient();
	bool IsAccpet();
	int	GetSocket();
	int Recv();
	int Send();
	virtual ~Net(void);
private:
	int socket;
	NetKind kind;
	bool closed;
};

#endif
