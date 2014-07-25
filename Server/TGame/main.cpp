#include "NetWork.h"

int main ()
{
	NetWork::getInstance()->Listen("0.0.0.0:80", 0, 0);
	while(true)
	{
		NetWork::getInstance()->Loop();
	}
}