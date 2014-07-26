#ifndef _NET_H
#define _NET_H

class Net
{
	enum NetKind
	{
		Server=1,
		Accept,
		Client,
		Unknow,
	};
public:
	Net(void);
	void check();
	bool isClosed();
	void server(int);
	bool isInvalid();
	virtual ~Net(void);
private:
	int socket;
	NetKind kind;
	bool closed;
};

#endif
