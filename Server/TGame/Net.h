#ifndef _NET_H
#define _NET_H

class Net
{
	enum NetKind
	{
		Server=1,
		Accept,
		Client,
		Unknow,
	};
public:
	Net(void);
	virtual ~Net(void);
};

#endif
