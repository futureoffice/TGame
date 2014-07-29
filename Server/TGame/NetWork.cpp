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
    inaddr.sin_family = AF_INET;        //ipv4协议族    
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
		select.Add(net);
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
	#ifdef _WIN32
		select.Add(newnet);
	#endif
	
	net->onAccept(newnet);
}

Net* NetWork::Connect(const char* host, Net::OnConnect onConnect, Net::OnClose onClose, unsigned int timeout)
{
	int port;
	char ip[16];
	if(sscanf(host, "%[^:]:%d", ip, &port)!=2)
		return 0;
	int sock = ::socket(PF_INET, SOCK_STREAM, 0);
	if(sock==INVALID_SOCKET)
		return 0;
	#ifdef _WIN32
		u_long mode = 1;
		if(ioctlsocket(sock,FIONBIO,&mode)!=0)
		{
			closesocket(sock);
			return 0;
		}
	#else
		int val = fcntl(sock, F_GETFL, 0);
		if (val == -1){return 0;}
		if (!(val & O_NONBLOCK))
		{
			if(fcntl(sock, F_SETFL, val | O_NONBLOCK | O_NDELAY)!=0)
				closesocket(sock);
				return 0;
		}
	#endif
	struct timeval tv = {timeout, 0};
    unsigned len = sizeof(tv);
    int ret = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO,(const char*)&tv, len);
    struct sockaddr_in inaddr;
    memset (&inaddr, 0,sizeof (struct sockaddr_in));
    inaddr.sin_family = AF_INET;        //ipv4协议族    
    inaddr.sin_port = htons (port);    
    if((!ip) || (ip[0] == 0) || (strcmp(ip, "0.0.0.0") == 0))
    {
        return 0;
    }
    else
    {
        inaddr.sin_addr.s_addr = inet_addr(ip);
    }
	if(::connect(sock, (struct sockaddr*)&inaddr, sizeof(struct sockaddr))==-1)
	{
		#ifdef _WIN32
			if(WSAGetLastError() == WSAEINPROGRESS )
			{
				printf("Connect timeout");
			}
		#endif
		return 0;
	}
	Net* net = new Net();
	net->Client(sock, onConnect, onClose);
	#ifdef _WIN32
		select.Add(net);
	#endif
	return net;
}

void NetWork::SyncConnect(Net* net)
{
	int optval = -1;
    int optlen = sizeof(optval);
	int socket = net->GetSocket();
    int ret = getsockopt( socket,SOL_SOCKET,SO_ERROR,(char*)(&optval),&optlen );
    if( ret == 0)
	{
		#ifdef _WIN32
			select.Mod(net, 1);
		#endif
		net->onConncet(net);
	}
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
				select.Del(net);
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
