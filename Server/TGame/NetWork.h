#pragma once
#ifndef _NetWork_H_
#define _NetWork_H_
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
	void start();
	Net Listen(const char* host, OnAccept onAccept, OnClose onClose);
	void Loop();
	void ReleaseNet(Net);
	~NetWork(void);
private:
	std::list<Net> nets;
};

#endif