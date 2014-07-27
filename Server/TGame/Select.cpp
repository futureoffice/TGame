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
int Select::Add(SOCKET socket)
{
    FD_SET(socket, &rset);
    return 0;
}
int Select::Del(SOCKET socket)
{
    FD_CLR(socket, &rset);
    return 0;
}
int Select::Mod(SOCKET socket)
{
	 if (FD_ISSET(socket, &rset))
		FD_CLR(socket, &rset);
	 else
		FD_SET(socket, &rset);
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
	std::list<Net> nets = NetWork::getInstance()->GetNets();
	std::list<Net>::iterator it;
	for(it=nets.begin();it!=nets.end();++it)
	{
		SOCKET sock = (*it).GetSocket();
        if (FD_ISSET(sock, &rset_out))
        {
			if((*it).IsServer())
			{
				NetWork::getInstance()->Accpet(*it);
			}
			else
			{
				(*it).Recv();
			}
		}
	}
	return n;
}