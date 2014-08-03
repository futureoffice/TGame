#include "NetWork.h"
#include "Net.h"
#include <iostream>
using namespace std;

int onReceive(Net* net, const char* data, unsigned int len)
{
	cout<<"received data:"<<data<<endl;
	return 0;
}

int onClose(Net* net, const char* msg)
{
	return 0;
}

void onAccept(Net* net)
{
	cout<<"Accept a Connect to the server"<<endl;
	net->SetOnReceive(onReceive, onClose);
}

int main ()
{
	NetWork::getInstance()->Start();
	NetWork::getInstance()->Listen("0.0.0.0:80", onAccept, onClose);
	cout<<"Server is Listening on:0.0.0.0:80"<<endl;
	while(true)
	{
		NetWork::getInstance()->Loop();
	}
}