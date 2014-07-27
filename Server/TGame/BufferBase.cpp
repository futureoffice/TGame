#include "BufferBase.h"


BufferBase::BufferBase(void)
{
}

int BufferBase::GetSize()
{
	return len;
}

int BufferBase::ShiftLeft(int num)
{
	return 0;
}

char* BufferBase::GetData()
{
	return data;
}

BufferBase::~BufferBase(void)
{
}
