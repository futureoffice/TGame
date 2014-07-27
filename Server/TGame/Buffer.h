#ifndef _BUFFER_H_
#define _BUFFER_H_
class Buffer
{
public:
	Buffer();
	~Buffer();
	char* GetData();
	unsigned int GetLeft();
	unsigned int ShiftLeft();
	void AddNum(int num);

private:
	unsigned int mLen;
	unsigned int mNum;
	unsigned int mOffset;

	char* mData;
};

#endif