#include "dbbase.h"
using namespace MYDB;

ConnItem::ConnItem()
{
	m_iIndex  =  -1;
	m_iPort   =  0;
	m_iStatus =  -2;

	m_pLink = NULL;
}

ConfInfo::ConfInfo()
{
	m_iIndex  =  0;
	m_iPort   =  0;
	m_iStatus =  0;
}

DBConn::DBConn()
{
	m_iErrorNo     =  0;
	m_strErrorInfo =  "OK";
}

DBConn::~DBConn()
{
	destroyConn();
}

int DBConn::init(const std::string& ip, const std::string& port, const std::string& pwd, const std::string& user)
{
	if (loadConf(ip, port, pwd, user) < 0)
	{
		return -1;
	}

	if (initConn() < 0)
	{
		return -1;
	}

	return 0;
}

int DBConn::loadConf(const std::string& ip, const std::string& port, const std::string& pwd, const std::string& user)
{
	ConfInfo confInfo;

	confInfo.m_strHost   =  ip;
	confInfo.m_iPort     =  atoi(port.c_str());
	confInfo.m_strPasswd =  pwd;
	confInfo.m_strUser   =  user;

	confInfo.m_iStart =  0;
	confInfo.m_iEnd   =  0;

	confInfo.m_iStatus =  0;
	confInfo.m_iIndex  =  0;

	m_vConfInfo.push_back(confInfo);

	m_iErrorNo     =  0;
	m_strErrorInfo =  "OK";

	return 0;
}

int DBConn::initConn()
{
	std::vector<ConfInfo>::iterator iter = m_vConfInfo.begin();
	for (; iter != m_vConfInfo.end(); ++iter)
	{
		ConnItem connItem;
		connItem.m_strHost   =  iter->m_strHost;
		connItem.m_strUser   =  iter->m_strUser;
		connItem.m_strPasswd =  iter->m_strPasswd;
		connItem.m_iPort     =  iter->m_iPort;

		connItem.m_pLink = mysql_init(NULL);
		if (!connItem.m_pLink)
		{
			m_iErrorNo     =  DB_INIT_ERR;
			m_strErrorInfo =  "init DB ERROR";
			destroyConn();
			return -1;
		}

		connItem.m_iStatus = -1;

		int timeout = 3;
		mysql_options(connItem.m_pLink,   MYSQL_OPT_CONNECT_TIMEOUT,   (const char *)&timeout);
		mysql_options(connItem.m_pLink,   MYSQL_OPT_READ_TIMEOUT,      (const char *)&timeout);
		mysql_options(connItem.m_pLink,   MYSQL_OPT_WRITE_TIMEOUT,     (const char *)&timeout);
		if (connect(connItem) < 0)
		{
			destroyConn();
			return -1;
		}

		connItem.m_iIndex  =  iter->m_iIndex;
		connItem.m_iStatus =  0;

		m_vConn.push_back(connItem);
	}

	return 0;
}

void DBConn::destroyConn()
{
	std::vector<ConnItem>::iterator iter = m_vConn.begin();
	for (; iter != m_vConn.end(); ++iter)
	{
		if (iter->m_pLink)
		{
			mysql_close(iter->m_pLink);
			iter->m_pLink = NULL;
		}
	}
}

int DBConn::connect(ConnItem &item)
{
	if (item.m_pLink)
	{
		mysql_close(item.m_pLink);
		item.m_pLink = NULL;
	}

	item.m_pLink = mysql_init(NULL);
	if (!item.m_pLink)
	{
		m_iErrorNo     =  DB_INIT_ERR;
		m_strErrorInfo =  "init DB ERROR";
		return -1;
	}

	int timeout = 3;
	mysql_options(item.m_pLink,   MYSQL_OPT_CONNECT_TIMEOUT,   (const char *)&timeout);
	mysql_options(item.m_pLink,   MYSQL_OPT_READ_TIMEOUT,      (const char *)&timeout);
	mysql_options(item.m_pLink,   MYSQL_OPT_WRITE_TIMEOUT,     (const char *)&timeout);

	if (!mysql_real_connect(item.m_pLink, item.m_strHost.c_str(), item.m_strUser.c_str(), item.m_strPasswd.c_str(),
	                        NULL, item.m_iPort, NULL, 0))
	{
		m_iErrorNo     =  DB_CONNECTION_ERR;
		m_strErrorInfo =  mysql_error(item.m_pLink);

		mysql_close(item.m_pLink);
		item.m_pLink = NULL;
		return -1;
	}

	return 0;

}

int DBConn::reconnect(int error_no)
{
	int index = 0;
	if (error_no == CR_SERVER_GONE_ERROR || error_no == CR_SERVER_LOST)
	{
		return connect(m_vConn[index]);
	}

	return 0;
}

MYSQL *DBConn::getConn()
{
	m_iErrorNo     =  0;
	m_strErrorInfo =  "OK";

	if (m_vConfInfo.empty() == true)
	{
		char msg[128] = {0};
		snprintf(msg, sizeof(msg), "the DB can't find, please check the db conf");

		m_iErrorNo     =  DB_NOT_EXIST;
		m_strErrorInfo =  msg;

		return NULL;
	}

	int index = 0;

	if (!m_vConn[index].m_pLink)
	{
		if (connect(m_vConn[index])) {
			return NULL;
		}
	}

	return m_vConn[index].m_pLink;
}

int DBConn::getErrorNo()
{
	return m_iErrorNo;
}

const char *DBConn::getErrorInfo()
{
	return m_strErrorInfo.c_str();
}

int DBConn::execQuery(const char *szSql, MYSQL *mysql, MYSQL_RES *&result_set)
{
	m_iErrorNo     =  0;
	m_strErrorInfo =  "OK";

	if (0 != mysql_query(mysql, szSql))
	{
		int iError = mysql_errno(mysql);

		char szError[128] = {0};
		snprintf(szError, sizeof(szError) - 1, "mysql_query ERROR[%d][%s]", mysql_errno(mysql), mysql_error(mysql));

		reconnect(iError);

		m_strErrorInfo =  szError;
		m_iErrorNo     =  iError;
		return -1;
	}

	if (NULL == (result_set = mysql_store_result(mysql)))
	{
		int iError = mysql_errno(mysql);

		char szError[128] = {0};
		snprintf(szError, sizeof(szError) - 1, "mysql_store_result ERROR[%d][%s]", mysql_errno(mysql), mysql_error(mysql));

		reconnect(iError);

		m_strErrorInfo =  szError;
		m_iErrorNo     =  iError;
		return -1;
	}

	return 0;
}

int DBConn::execInsertUpdate(const char *szSql, MYSQL *mysql)
{
	m_iErrorNo     =  0;
	m_strErrorInfo =  "OK";

	if (0 != mysql_query(mysql, szSql))
	{
		int iError = mysql_errno(mysql);

		char szError[128] = {0};
		snprintf(szError, sizeof(szError), "mysql_query ERROR[%d][%s]", mysql_errno(mysql), mysql_error(mysql));

		reconnect(iError);

		m_strErrorInfo =  szError;
		m_iErrorNo     =  iError;
		return -1;
	}

	return 0;
}

int DBConn::execInsertUpdate(const char *szSql, MYSQL *mysql, int &iAffectRow)
{
	m_iErrorNo     =  0;
	m_strErrorInfo =  "OK";
	iAffectRow     =  0;

	if (0 != mysql_query(mysql, szSql))
	{
		int iError = mysql_errno(mysql);
		iAffectRow = mysql_affected_rows(mysql);

		char szError[128] = {0};
		snprintf(szError, sizeof(szError) - 1, "mysql_query ERROR[%d][%s]", mysql_errno(mysql), mysql_error(mysql));

		reconnect(iError);

		m_strErrorInfo =  szError;
		m_iErrorNo     =  iError;
		return -1;
	}

	iAffectRow = mysql_affected_rows(mysql);

	return 0;
}

void DBConn::ping(void)
{
	std::vector<ConnItem>::iterator iter = m_vConn.begin();
	for (; iter != m_vConn.end(); ++iter)
	{
		if (iter->m_pLink == NULL)
		{
			continue;
		}
		mysql_ping(iter->m_pLink);
	}
}

