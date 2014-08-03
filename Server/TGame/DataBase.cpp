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
	long long pis[128], cis[128];
	memset(st->params, 0, pn * sizeof(MYSQL_BIND));
	for (int p = 1; p <= pn; p++)
	{
		MYSQL_BIND *b = st->params+p-1;
		b->buffer = pis+p;
		mysqlEnc();
	}
	if (mysql_stmt_bind_param(st, st->params))
	{
		printf("sql error: bind param ", pn);
		return;
	}
	if (cn > 0)
	{
		memset(st->bind, 0, cn * sizeof(MYSQL_BIND));
		if (mysql_stmt_bind_result(st, st->bind))
			mysqlErr(lua, upx(1), sql, "sql error: %s", st, 0);
	}
	if (mysql_stmt_execute(st))
		mysqlErr(lua, upx(1), sql, "sql error: %s", st, 0);
	if (cn == 0)
		return pushn(lua, mysql_stmt_affected_rows(st)), pushn(lua, mysql_stmt_insert_id(st)), 2;

	int D = gettop(lua)+1, c;
	if (cols)
		for (newtablen(lua, 0, cn), c = 1; c <= cn; c++) // D data
			isnil(lua, L+1) || (rawgeti(lua, L+3, c), rawget(lua, L+1), popz(lua)) // !exclude[prepare[c]]
			? newtablen(lua, 8, 0), rawgeti(lua, L+3, c), rawsetv(lua, D, -2) : pushz(lua); // D+c cols
	else
		newtablen(lua, 8, 0); // D data
	for (int r = 1; ; r++)
	{
		memset(st->bind, 0, cn * sizeof(MYSQL_BIND));
		for (c = 1; c <= cn; c++)
		{
			MYSQL_BIND *b = st->bind+c-1;
#if WIN
			b->buffer_type = (enum_field_types)cts[c];
#else
			b->buffer_type = (enum_field_types)cts[c];
#endif
			b->buffer = cis+c, cis[c] = 0;
			b->buffer_length = cts[c]==MYSQL_TYPE_STRING || cts[c]==MYSQL_TYPE_BLOB ? 0 : 8;
		}
		mysql_stmt_bind_result(st, st->bind);
		int fail = mysql_stmt_fetch(st);
		if (fail == MYSQL_NO_DATA)
			break;
		if (fail && fail != MYSQL_DATA_TRUNCATED)
			mysqlErr(lua, upx(1), sql, "sql error: %s", st, 2);
		if ( !cols)
			newtablen(lua, 0, cn); // row
		for (c = 1; c <= cn; c++)
		{
			MYSQL_BIND *b = st->bind+c-1;
			if ((int)b->buffer_type != cts[c])
				err = mysql_stmt_error(st), mysql_stmt_free_result(st),
				error(lua, "sql error: different column type %d %d", cts[c], b->buffer_type);
			if (isnil(lua, L+1) || (rawgeti(lua, L+3, c), rawget(lua, L+1), popz(lua))) // !exclude[prepare[c]]
			{
				cols || (rawgeti(lua, L+3, c), 0); // column name
				mysqlDec(lua, upx(1), sql, st, b, c);
				cols ? rawseti(lua, D+c, r) : rawset(lua, -3);
			}
		}
		if ( !cols)
			rawseti(lua, D, r);
	}
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
