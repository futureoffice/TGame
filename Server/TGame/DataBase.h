#ifndef _DATABASE_H_
#define _DATABASE_H_
#include <mysql.h>
#include <hash_map>
#include <vector>

class VarType
{
	enum DataType
	{

	};
public:
	DataType type;
	union value
	{
		int mint;
		char* mchar;
		float mfloat;
		double mdouble;
		long long mlong;
	};
};

class DataBase
{
public:
	DataBase();
	~DataBase();
	void Init();
	void Connect(const char* host, unsigned int port, const char* username, const char* password, const char* dbname,unsigned int timeou);
	void Execute(const char* sql, std::vector<int, VarType*>* args);
	void Close(const char* reason);
	bool IsClosed();
private:
	struct MysqlPreType
	{
		MYSQL_STMT* stmt;
		std::vector<int, unsigned int> types;
	};
	MYSQL* mConn;
	bool mPrepare;
	std::hash_map<const char*, MysqlPreType*> stmts;
};

#endif //_DATABASE_H_
