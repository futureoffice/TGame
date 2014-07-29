#include "NetWork.h"
#include "Assert.h"
NetWork* NetWork::Singleton<NetWork>::_instance=0;

NetWork::NetWork(void)
{
	
}

void NetWork::Start()
{
#ifdef _WIN32
	WSADATA     wsaData;  
    WSAStartup(0x0202, &wsaData);  
#endif
}

Net* NetWork::Listen(const char* host, Net::OnAccept onAccept, Net::OnClose onClose)
{
	int port;
	char ip[16];
	if(sscanf(host, "%[^:]:%d", ip, &port)!=2)
		return 0;
	int sock = ::socket(PF_INET, SOCK_STREAM, 0);
	if(sock==INVALID_SOCKET)
		return 0;
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
	int bret = ::bind(sock, (struct sockaddr*)&inaddr, sizeof(struct sockaddr));
    if(bret!=0)
		return 0;
    if(listen(sock, 32) != 0)
        return 0;
	Net* net = new Net();
	net->Server(sock, onAccept, onClose);
	#ifdef _WIN32
		select.Add(sock);
	#endif
	return net;
}

void NetWork::Accept(Net* net)
{
	TAssert(net->IsServer(), "Only Listen Socket Can Accept!");
	SOCKET newsock = ::accept(net->GetSocket(), NULL, NULL);
	if( newsock < 0 )
	{
		return;
	}
	Net* newnet = new Net;
	newnet->Accept(newsock);
	nets.push_back(newnet);
	select.Add(newsock);
	
	net->onAccept(newnet);
}

void NetWork::Loop()
{
	select.Dispatch(0);
	std::list<Net*>::iterator it;
	for(it=nets.begin();it!=nets.end();)
	{
		Net* net=*it;
		if(net->IsClosed())
		{
			#ifdef _WIN32
				select.Del(net->GetSocket());
			#endif
			nets.erase(it++);
			delete net;
		}
	}
}

std::list<Net*> NetWork::GetNets()
{
	return nets;
}

NetWork::~NetWork(void)
{
}
