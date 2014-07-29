#ifndef _NetWork_H_
#define _NetWork_H_
#ifdef _WIN32
#include <winsock2.h>
#include "Select.h"
#else
typedef int SOCKET;
#endif
#include "Singleton.h"
#include "Net.h"
#include <stdio.h>
#include <list>

class NetWork: public Singleton<NetWork>
{
	friend class Singleton<NetWork>;
	
protected:
	NetWork(void);
public:
	void Start();
	Net* Listen(const char* host, Net::OnAccept onAccept, Net::OnClose onClose);
	void Accept(Net* net);
	Net* Connect(const char* host, Net::OnConnect onConnect, Net::OnClose onClose, unsigned int timeout);
	void SyncConnect(Net* net);
	void Loop();
	std::list<Net*>  GetNets();
	void ReleaseNet(Net*);
	~NetWork(void);
private:
	std::list<Net*> nets;
#ifdef _WIN32
	Select select;
#endif
};

#endif