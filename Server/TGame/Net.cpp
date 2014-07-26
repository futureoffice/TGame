#include "Net.h"


Net::Net(void)
	:socket(0),kind(Unknow),closed(false)
{
}

void Net::check()
{
	if(kind==NetKind::Server)
	{

	}else
	{
	}
}

void Net::server(int socket)
{
	this->socket=socket;
	this->closed=false;
	this->kind=NetKind::Server;
}

bool Net::isInvalid()
{
	return kind==NetKind::Unknow;
}

bool Net::isClosed()
{
	return this->closed;
}

Net::~Net(void)
{
}
