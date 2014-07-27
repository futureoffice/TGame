#ifndef _SELECT_H_
#define _SELECT_H_
#include <WinSock2.h>

class Select
{
public:
	Select(void);
	~Select(void);
	int Add(SOCKET socket);
	int Del(SOCKET socket);
	int Mod(SOCKET socket);
	int Dispatch(int timeout);
private:
	fd_set rset_out;
	fd_set wset_out;

    fd_set rset;
    fd_set wset;
};
#endif //_SELECT_H_