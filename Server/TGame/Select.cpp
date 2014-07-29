#include "Select.h"
#include "NetWork.h"
Select::Select()
{
    FD_ZERO(&rset);
    FD_ZERO(&wset);
    FD_ZERO(&rset_out);
    FD_ZERO(&wset_out);
}
Select::~Select(){}
int Select::Add(Net* net)
{
	SOCKET socket = net->GetSocket();
    FD_SET(socket, &rset);
	if(net->IsClient())
		FD_SET(socket, &wset);
    return 0;
}
int Select::Del(Net* net)
{
	SOCKET socket = net->GetSocket();
    FD_CLR(socket, &rset);
	if(net->IsClient())
		 FD_CLR(socket, &wset);
    return 0;
}
int Select::Mod(Net* net, int mod)
{
	SOCKET socket = net->GetSocket();
	if(mod == 0)
	{
		if (FD_ISSET(socket, &rset))
			FD_CLR(socket, &rset);
		else
			FD_SET(socket, &rset);
	}else
	{
		if (FD_ISSET(socket, &rset))
			FD_CLR(socket, &rset);
		else
			FD_SET(socket, &rset);
	}
	return 0;
}
int Select::Dispatch(int timeout)
{
    int tv_sec = timeout/1000;
    int tv_usec = (timeout*1000)%1000000;
    struct timeval tv = {tv_sec, tv_usec};
    memcpy(&rset_out, &rset, sizeof(rset));
    memcpy(&wset_out, &wset, sizeof(wset));
	int n = select(0, &rset_out,&wset_out,NULL,&tv);
    if(n < 0)
    {
		if(WSAEINVAL == SOCKET_ERROR)
		{
			Sleep(timeout);
		}
	}
    if(n <= 0)
    {
		return n;
	}
	std::list<Net*> nets = NetWork::getInstance()->GetNets();
	std::list<Net*>::iterator it;
	for(it=nets.begin();it!=nets.end();++it)
	{
		SOCKET sock = (*it)->GetSocket();
        if (FD_ISSET(sock, &rset_out))
        {
			if((*it)->IsServer())
			{
				NetWork::getInstance()->Accept(*it);
			}
			else if((*it)->IsClient())
			{
				NetWork::getInstance()->SyncConnect(*it);
			}
			else
			{
				(*it)->Recv();
			}
		}else if (FD_ISSET(sock, &wset_out) && (*it)->IsClient())
		{	
			NetWork::getInstance()->SyncConnect(*it);
		}
	}
	return n;
}