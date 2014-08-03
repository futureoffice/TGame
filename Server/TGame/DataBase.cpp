#include "DataBase.h"
#include "Assert.h"

DataBase::DataBase():mPrepare(true)
{
	Init();
}

void DataBase::Init()
{
	mConn = mysql_init((MYSQL*)0);
	TAssert(mConn, "Mysql Init Failed");
}

void DataBase::Connect(const char* host, unsigned int port, const char* username, const char* password, const char* dbname, unsigned int timeout)
{
	mysql_options(mConn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
	mysql_options(mConn, MYSQL_OPT_READ_TIMEOUT, &timeout);
	mysql_options(mConn, MYSQL_OPT_WRITE_TIMEOUT, &timeout);
	TAssert(mysql_real_connect(mConn, host, username, password, dbname, port, NULL, CLIENT_MULTI_STATEMENTS), "Mysql Connect Failed");
	mysql_set_character_set(mConn, "utf8");
}

void DataBase::Execute(const char* sql, std::vector<int, VarType*>* args)
{
	TAssert(mConn, "Mysql closed");
	MysqlPreType* pretype;
	MYSQL_STMT* st;
	int cn = 0;
	std::hash_map<const char*, MysqlPreType*>::const_iterator mIt;
	mIt = stmts.find(sql);
	if (mIt!=stmts.end())
	{
		pretype =(MysqlPreType*)(mIt->second);
		st = (MYSQL_STMT*)pretype->stmt;
	}
	else
	{
		st = mysql_stmt_init(mConn);
		TAssert(st, "Mysql PrepareStatement Init Failed");
		if (mysql_stmt_prepare(st, sql, strlen(sql)+1))
			TAssert(st, "Mysql PrepareStatement Prepare Failed");
		MYSQL_RES *res = mysql_stmt_result_metadata(st);
		if (mysql_stmt_errno(st))
			TAssert(st, "Mysql Get PrepareStatement meta Failed");
		if (res)
		{
			cn = mysql_num_fields(res);
			if (cn >= 128)
			{
				mysql_stmt_free_result(st);
				mysql_stmt_close(st);
				printf("sql error: too many columns %d", cn);
				return;
			}
			pretype = new MysqlPreType();
			MYSQL_FIELD* c;
			for (int i = 1; i <= cn; i++)
			{
				c = mysql_fetch_field_direct(res, i-1);
				unsigned ctype = c->type;
				switch (ctype)
				{
				case MYSQL_TYPE_TINY: case MYSQL_TYPE_SHORT: case MYSQL_TYPE_LONG:
				case MYSQL_TYPE_LONGLONG: case MYSQL_TYPE_FLOAT: case MYSQL_TYPE_DOUBLE:
					pretype->types.push_back(ctype); break;
				case MYSQL_TYPE_STRING: case MYSQL_TYPE_VAR_STRING:
					pretype->types.push_back(c->charsetnr == 63 ? MYSQL_TYPE_BLOB:ctype); break;  // binary
				case MYSQL_TYPE_BLOB:
					pretype->types.push_back(c->charsetnr == 63 ? MYSQL_TYPE_BLOB:ctype); break;  // blob
				default:
					mysql_stmt_free_result(st), mysql_stmt_close(st);
					printf("sql error: unknown type of column %d", i);
					exit;
				}
			}
			mysql_stmt_free_result(st);
		}
		else // no result set
			pretype = new MysqlPreType();
		pretype->stmt = st;
		stmts[sql]=pretype;
	}

	int pn = mysql_stmt_param_count(st);
	if (pn >= 128)
	{
		printf("sql error: too many parameters %d", pn);
		return;
	}
	int paramn = args->size();
	if (pn != paramn )
		printf("bad argument (%d parameters expected, got %d)", pn, paramn);
}

void DataBase::Close(const char* reason)
{
	if(!mConn)
		return;
	if(reason)
		printf(reason);
	if(mPrepare)
		mysql_stmt_close(mstmt);
	mysql_close(mConn);
	mConn = 0;
}

bool DataBase::IsClosed()
{
	return (!mConn);
}

DataBase::~DataBase()
{
}
