#ifndef _NETBUFFER_H
#define _NETBUFFER_H
#include "BufferBase.h"
#include "Net.h"
#define BUFFERSIZE
class NetBuffer
{
public:
	NetBuffer(void);
	void Read(Net net);
	~NetBuffer(void);
private:
	BufferBase* buffer;
	unsigned int offset;
};

#endif