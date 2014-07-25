#pragma once
#ifndef _NetWork_H_
#define _NetWork_H_
#include "Singleton.h"
#include "Net.h"

class NetWork: public Singleton<NetWork>
{
	friend class Singleton<NetWork>;
	typedef void OnAccept(Net*);
	typedef void OnClose(Net*);
	
public:
	NetWork(void);
	Net* Listen(const char* host, OnAccept onAccept, OnClose onClose);
	void Loop();
	~NetWork(void);
};

#endif