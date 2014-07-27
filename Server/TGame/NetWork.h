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
	typedef void OnAccept(Net);
	typedef void OnClose(Net);
	
protected:
	NetWork(void);
public:
	void Start();
	Net Listen(const char* host, OnAccept onAccept, OnClose onClose);
	void Accpet(Net net);
	void Loop();
	std::list<Net>  GetNets();
	void ReleaseNet(Net);
	~NetWork(void);
private:
	std::list<Net> nets;
#ifdef _WIN32
	Select select;
#endif
};

#endif