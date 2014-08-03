#include <stdio.h>
#include <stdlib.h>
#ifdef _DEBUG
	#define TAssert(cond, msg) if(!(cond)) { printf(msg); exit(1); } 
#else
	#define TAssert(cond, msg) ((void)0)
#endif