#include "Net.h"
#ifdef _WIN32
#include <winsock2.h>
#endif

Net::Net(void)
	:mSocket(0),mKind(UNKNOW),mClosed(false),mBuffer(0)
{
	onAccept = 0;
	onReceive = 0;
	onClose = 0;
}

void Net::Server(int socket, OnAccept* onAccept, OnClose* onClose)
{
	mSocket=socket;
	mClosed=false;
	mKind=NetKind::SERVER;
	onAccept = onAccept;
	onClose = onClose;
}

bool Net::IsInvalid()
{
	return mKind==NetKind::UNKNOW;
}

bool Net::IsServer()
{
	return mKind==NetKind::SERVER;
}

bool Net::IsClient()
{
	return mKind==NetKind::CLIENT;
}

bool Net::IsAccpet()
{
	return mKind==NetKind::ACCEPT;
}

int Net::GetSocket()
{
	return mSocket;
}

void Net::Accept(int socket)
{
	mSocket=socket;
	mClosed=false;
	mKind=NetKind::ACCEPT;
	mBuffer = new Buffer;
}

int Net::Recv()
{
	int left = mBuffer->GetLeft();
	int n = ::recv(mSocket, mBuffer->GetData(), left, 0);
	if(n<=0)
	{
		Close();
		return n;
	}
	mBuffer->AddNum(n);
	if(n>=left)
	{
		mBuffer->ShiftLeft();
		left = mBuffer->GetLeft();
		n = ::recv(mSocket, mBuffer->GetData(), left, 0);
	}
	if(onReceive)
	{
		onReceive(this, mBuffer->GetData(), n);
	}
	return n;
}

int Net::Send()
{
	return 0;
}

int Net::Close()
{
	closesocket(mSocket);
	return 0;
}

bool Net::IsClosed()
{
	return mClosed;
}

void Net::SetOnReceive(OnReceive onReceive, OnClose onClose)
{
	this->onReceive = onReceive;
	this->onClose = onClose;
}

Net::~Net()
{
	delete mBuffer;
}
