#ifndef _BUFFERBASE_H
#define _BUFFERBASE_H

class BufferBase
{
public:
	BufferBase(void);
	int GetSize();
	char* GetData();
	int ShiftLeft(int num);
	~BufferBase(void);
private:
	char* data;
	unsigned int len;
};

#endif