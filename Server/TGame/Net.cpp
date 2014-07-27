#include "Net.h"


Net::Net(void)
	:socket(0),kind(UNKNOW),closed(false)
{
}

void Net::Server(int socket)
{
	this->socket=socket;
	this->closed=false;
	this->kind=NetKind::SERVER;
}

bool Net::IsInvalid()
{
	return kind==NetKind::UNKNOW;
}

bool Net::IsServer()
{
	return kind==NetKind::SERVER;
}

bool Net::IsClient()
{
	return kind==NetKind::CLIENT;
}

bool Net::IsAccpet()
{
	return kind==NetKind::ACCEPT;
}

int Net::GetSocket()
{
	return socket;
}

void Net::Accpet(int socket)
{
	this->socket=socket;
	this->closed=false;
	this->kind=NetKind::ACCEPT;
}

int Net::Recv()
{
	return 0;
}

int Net::Send()
{
	return 0;
}

bool Net::IsClosed()
{
	return this->closed;
}

Net::~Net(void)
{
}
