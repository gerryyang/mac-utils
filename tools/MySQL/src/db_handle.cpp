#include <sys/time.h>
#include <time.h>
#include "db_handle.h"

using namespace MYDB;

int CMyDB::Init(const std::string& ip, const std::string& port, const std::string& pwd, const std::string& user)
{
	int ret     =  E_OK;
	ret         =  m_dbconn.init(ip, port, pwd, user);
	m_errno     =  m_dbconn.getErrorNo();
	m_errinfo   =  m_dbconn.getErrorInfo();

	return ret;
}

int CMyDB::GetErrorNo()
{
	return m_errno;
}

string CMyDB::GetErrorInfo()
{
	return m_errinfo;
}

int CMyDB::Query()
{
	int ret = E_OK;

	MYSQL *mysql = m_dbconn.getConn();

	std::string sql = "select * from order_center.t_order_state limit 1;";

	MYSQL_RES *result;
	ret = m_dbconn.execQuery(sql.c_str(), mysql, result);
	if (ret != E_OK) {
		printf("execQuery err[%d:%s]\n", m_dbconn.getErrorNo(), m_dbconn.getErrorInfo());
		return E_ERR;
	}

	MYSQL_ROW row;
	MYSQL_FIELD *field;
	int num_fields = mysql_field_count(mysql);
	while ((row = mysql_fetch_row(result)) != NULL)
	{
		for (int i = 0; i < num_fields; ++i)
		{
			field = mysql_fetch_field_direct(result, i);
			printf("%s: %s, ", field->name, row[i]);
		}
		printf("\n");
	}
	mysql_free_result(result);

	return E_OK;
}

int CMyDB::Insert()
{
	int ret = E_OK;

	MYSQL *mysql = m_dbconn.getConn();

	std::string sql = "insert into order_center.t_order_state(FOfferId,FOrderId,FSystemName,FOrderState) values('123456', 'gerry_123_1', 'portal', '910');";

	int affect_row = 0;
	ret = m_dbconn.execInsertUpdate(sql.c_str(), mysql, affect_row);
	if (ret != E_OK) {
		printf("execInsertUpdate err[%d:%s]\n", m_dbconn.getErrorNo(), m_dbconn.getErrorInfo());
		return E_ERR;
	}
	printf("affect_row[%d]\n", affect_row);

	return E_OK;
}

int CMyDB::CheckPortalOrderExist(const std::string& OrderId)
{
	int ret = E_OK;

	std::string FOfferId  = "1450008583";
	std::string FOrderId  = OrderId;

	MYSQL *mysql = m_dbconn.getConn();

	std::string sql = "select * from order_center.t_order_state where FOfferId = '" + FOfferId + "' and FOrderId = '" + FOrderId + "'and FSystemName = 'portal';";

	MYSQL_RES *result;
	ret = m_dbconn.execQuery(sql.c_str(), mysql, result);
	if (ret != E_OK) {
		printf("execQuery err[%d:%s]\n", m_dbconn.getErrorNo(), m_dbconn.getErrorInfo());
		return E_ERR;
	}

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(result)) != NULL)
	{
		printf("Error, FOfferId[%s] FOrderId[%s] existed\n", FOfferId.c_str(), FOrderId.c_str());
		return E_ERR;
	}
	mysql_free_result(result);

	return E_OK;
}

int CMyDB::CheckChannelOrderExist(const std::string& OrderId)
{
	int ret = E_OK;

	std::string FOfferId  = "1450008583";
	std::string FOrderId  = OrderId;

	MYSQL *mysql = m_dbconn.getConn();

	std::string sql = "select * from order_center.t_order_state where FOfferId = '" + FOfferId + "' and FOrderId = '" + FOrderId + "'and FSystemName = 'portal-remitpay';";

	MYSQL_RES *result;
	ret = m_dbconn.execQuery(sql.c_str(), mysql, result);
	if (ret != E_OK) {
		printf("execQuery err[%d:%s]\n", m_dbconn.getErrorNo(), m_dbconn.getErrorInfo());
		return E_ERR;
	}

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(result)) != NULL)
	{
		printf("Error, FOfferId[%s] FOrderId[%s] existed\n", FOfferId.c_str(), FOrderId.c_str());
		return E_ERR;
	}
	mysql_free_result(result);

	return E_OK;
}

static std::string get_random()
{
	const char *prefix = "resume";
	char lotime[36] = {0};
	time_t now = time(NULL);
	struct timeval now2;
	gettimeofday(&now2, NULL);
	strftime(lotime, sizeof(lotime), "%Y%m%d-%H%M%S", localtime(&now));
	snprintf(lotime + strlen(lotime), sizeof(lotime) - strlen(lotime), "%lu", now2.tv_usec);
	char psn[64] = {0};
	static unsigned cnt = 1;
	snprintf(psn, sizeof(psn), "%s-%.18s%d", prefix, lotime, cnt++ % 10);
	return psn;
}

int CMyDB::InsertPortalOrder(const std::string& OrderId, const std::string& OrderExtended)
{
	int ret = E_OK;

	ret = CheckPortalOrderExist(OrderId);
	if (ret != E_OK) {
		return E_ERR;
	}

	MYSQL *mysql = m_dbconn.getConn();

	// infos
	std::string FOfferId          = "1450008583";
	std::string FSystemName       = "portal";
	std::string FInnerOrderId     = get_random();
	std::string FPayWay           = "remitpay";
	std::string FPayChannel       = "remitpay";
	std::string FPayChannelSubId  = "1";
	std::string FOrderState       = "910";

	// req params
	std::string FOrderId        = OrderId;
	std::string FOrderExtended  = OrderExtended;

	std::string sql = "insert into order_center.t_order_state(FOfferId, FOrderId, FSystemName, FInnerOrderId, FOrderState, FPayWay, FPayChannel, FPayChannelSubId, FOrderExtended) values('"
	                  + FOfferId + "', '"
	                  + FOrderId + "', '"
	                  + FSystemName + "', '"
	                  + FInnerOrderId + "', '"
	                  + FOrderState + "', '"
	                  + FPayWay + "', '"
	                  + FPayChannel + "', '"
	                  + FPayChannelSubId + "', '"
	                  + FOrderExtended + "' "
	                  ");";

	printf("sql[%s]\n", sql.c_str());
	int affect_row = 0;
	ret = m_dbconn.execInsertUpdate(sql.c_str(), mysql, affect_row);
	if (ret != E_OK) {
		printf("execInsertUpdate err[%d:%s]\n", m_dbconn.getErrorNo(), m_dbconn.getErrorInfo());
		return E_ERR;
	}
	printf("affect_row[%d]\n", affect_row);

	return E_OK;
}

int CMyDB::InsertChannelOrder(const std::string& OrderId, const std::string& OrderExtended)
{
	int ret = E_OK;

	ret = CheckChannelOrderExist(OrderId);
	if (ret != E_OK) {
		return E_ERR;
	}

	MYSQL *mysql = m_dbconn.getConn();

	// infos
	std::string FOfferId          = "1450008583";
	std::string FSystemName       = "portal-remitpay";
	std::string FInnerOrderId     = get_random();
	std::string FPayWay           = "remitpay";
	std::string FPayChannel       = "remitpay";
	std::string FPayChannelSubId  = "1";
	std::string FOrderState       = "910";

	// req params
	std::string FOrderId        = OrderId;
	std::string FOrderExtended  = OrderExtended;

	std::string sql = "insert into order_center.t_order_state(FOfferId, FOrderId, FSystemName, FInnerOrderId, FOrderState, FPayWay, FPayChannel, FPayChannelSubId, FOrderExtended) values('"
	                  + FOfferId + "', '"
	                  + FOrderId + "', '"
	                  + FSystemName + "', '"
	                  + FInnerOrderId + "', '"
	                  + FOrderState + "', '"
	                  + FPayWay + "', '"
	                  + FPayChannel + "', '"
	                  + FPayChannelSubId + "', '"
	                  + FOrderExtended + "' "
	                  ");";

	printf("sql[%s]\n", sql.c_str());
	int affect_row = 0;
	ret = m_dbconn.execInsertUpdate(sql.c_str(), mysql, affect_row);
	if (ret != E_OK) {
		printf("execInsertUpdate err[%d:%s]\n", m_dbconn.getErrorNo(), m_dbconn.getErrorInfo());
		return E_ERR;
	}
	printf("affect_row[%d]\n", affect_row);

	return E_OK;
}

