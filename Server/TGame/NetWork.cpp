#include "NetWork.h"
#ifdef _WIN32
#include <winsock2.h>
#endif

NetWork* NetWork::Singleton<NetWork>::_instance=0;

NetWork::NetWork(void)
{
	
}

void NetWork::start()
{
#ifdef _WIN32
	WSADATA     wsaData;  
    WSAStartup(0x0202, &wsaData);  
#endif
}

Net NetWork::Listen(const char* host, OnAccept onAccept, OnClose onclose)
{
	Net net;
	int port;
	char ip[16];
	if(sscanf("%s:%d", ip, &port))
		return net;
	int sock = ::socket(PF_INET, SOCK_STREAM, 0);
	int optval = 1;
    unsigned optlen = sizeof(optval);
    int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, optlen);
    struct sockaddr_in inaddr;    
    memset (&inaddr, 0,sizeof (struct sockaddr_in));
    inaddr.sin_family = AF_INET;        //ipv4Ð­Òé×å    
    inaddr.sin_port = htons (port);    
       if((!ip) || (ip[0] == 0) || (strcmp(ip, "0.0.0.0") == 0))
    {
        inaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        inaddr.sin_addr.s_addr = inet_addr(ip);
    }
    if(::bind(sock, (struct sockaddr*)&inaddr, sizeof(struct sockaddr))!=0)
		return net;
    if(listen(sock, 32) != 0)
        return net;
	net.server(sock);
	return net;
}

void NetWork::Loop()
{
	std::list<Net>::iterator it;
	for(it=nets.begin();it!=nets.end();)
	{
		Net net=*it;
		if(net.isClosed())
		{
			nets.erase(it++);
		}else{
			net.check();
			it++;
		}
	}
}

NetWork::~NetWork(void)
{
}
