#include "Buffer.h"
#include <string.h>
#define BUFFMAXLEN 65536

Buffer::Buffer():mLen(BUFFMAXLEN), mOffset(0), mNum(0)
{
	mData = new char(mLen);
}

Buffer::~Buffer()
{
	delete mData;
}

char* Buffer::GetData()
{
	return mData+mOffset+mNum;
}

unsigned int Buffer::GetLeft()
{
	return mLen-mOffset-mNum;
}

unsigned int Buffer::ShiftLeft()
{
	if(mOffset<=0)
		return 0;
	unsigned int offset = mOffset;
	memmove(mData, mData+mOffset, mNum);
	mOffset = 0;
	return offset;
}

void Buffer::AddNum(int num)
{
	mNum += num;
}