_Pragma("once")

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mysql.h"
#include <errmsg.h>
#include <string>
#include <vector>
#include <map>
using namespace std;

#define Record map<string, string>

#define E_OK 0
#define E_ERR -1

// 1000~2999 mysql errno

#define DB_CONF_NOT_EXIST				3001
#define DB_INIT_ERR						3002
#define DB_INDEX_ERR					3003
#define DB_NOT_EXIST					3004
#define DB_CONNECTION_ERR				3005
#define DB_FILE_PARSE_ERR				3006
#define DB_FETCH_ROW_ERR				3007

namespace MYDB
{
	struct ConnItem
	{
		int m_iStatus; // -2 初始状态, -1 完成init 操作, 0 完成connect 操作 1 被占用
		int m_iIndex;

		int m_iPort;
		std::string m_strHost;
		std::string m_strUser;
		std::string m_strPasswd;
		std::string m_strDBName;
		MYSQL *m_pLink;
		ConnItem();
	};

	struct ConfInfo
	{
		int m_iType;
		int m_iStatus;
		int m_iIndex;

		int m_iPort;

		int m_iStart;
		int m_iEnd;

		std::string m_strHost;
		std::string m_strUser;
		std::string m_strPasswd;
		ConfInfo();
	};

	class DBConn
	{
	private:
		std::vector<ConfInfo> m_vConfInfo;
		std::vector<ConnItem> m_vConn;

		std::string m_strConfigFile;

		int m_iErrorNo;
		std::string m_strErrorInfo;

	public:
		DBConn();
		~DBConn();

		int init(const std::string& ip, const std::string& port, const std::string& pwd, const std::string& user);

		MYSQL* getConn();

		int reconnect(int error_no);


		int execQuery(const char *szSql, MYSQL *mysql, MYSQL_RES *&result_set);

		int execInsertUpdate(const char *szSql, MYSQL *mysql);
		int execInsertUpdate(const char *szSql, MYSQL *mysql, int &iAffectRow);

		void ping();

		int getErrorNo();
		const char *getErrorInfo();

	private:

		DBConn(const DBConn &);
		DBConn &operator=(const DBConn &);

	private:
		int loadConf(const std::string& ip, const std::string& port, const std::string& pwd, const std::string& user);

		int initConn();

		int connect(ConnItem &item);

		void destroyConn();

	};
}

