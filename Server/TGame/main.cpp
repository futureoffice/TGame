#include "NetWork.h"
#include "Net.h"

int onReceive(Net* net, const char* data, unsigned int len)
{

	return 0;
}

int onClose(Net* net, const char* msg)
{
	return 0;
}

void onAccept(Net* net)
{
	net->SetOnReceive(onReceive, onClose);
}

int main ()
{
	NetWork::getInstance()->Start();
	NetWork::getInstance()->Listen("0.0.0.0:80", onAccept, onClose);
	while(true)
	{
		NetWork::getInstance()->Loop();
	}
}