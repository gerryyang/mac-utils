_Pragma("once")

#include "dbbase.h"

namespace MYDB 
{
	class CMyDB
	{
	public:
		CMyDB() {};
		~CMyDB() {};

		int GetErrorNo(void);
		std::string GetErrorInfo(void);

		int Init(const std::string& ip, const std::string& port, const std::string& pwd, const std::string& user);

		int Query();
		int Insert();
		int Update();

		int CheckPortalOrderExist(const std::string& OrderId);
		int InsertPortalOrder(const std::string& OrderId, const std::string& OrderExtended);

		int CheckChannelOrderExist(const std::string& OrderId);
		int InsertChannelOrder(const std::string& OrderId, const std::string& OrderExtended);

	private:
		int m_errno;
		std::string m_errinfo;

		DBConn m_dbconn;

	};
}

