#ifndef _SELECT_H_
#define _SELECT_H_
#include <WinSock2.h>
#include "Net.h"

class Select
{
public:
	Select(void);
	~Select(void);
	int Add(Net* net);
	int Del(Net* net);
	int Mod(Net* net, int mod);
	int Dispatch(int timeout);
private:
	fd_set rset_out;
	fd_set wset_out;

    fd_set rset;
    fd_set wset;
};
#endif //_SELECT_H_