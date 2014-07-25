#ifndef _Singleton_H_
#define _Singleton_H_

template<class T>
class Singleton
{
protected:
	Singleton(void){}
	static T* _instance;
public:
	static T* getInstance(){
		if(!_instance)
		{
			_instance=new T();
		}
		return _instance;
	}
	static void realseInstance()
	{
		if(!_instance) return;
		free(_instance);
	}
	virtual ~Singleton(void){}
};

#endif