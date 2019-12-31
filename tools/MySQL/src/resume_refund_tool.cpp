#include <iostream>
#include <cstdio>
#include <string>
#include "db_handle.h"

using namespace std;
using namespace MYDB;


int main(int argv, char** argc)
{
    if (argv != 7) {
        printf("usage: %s db_ip db_port db_pwd db_user FOrderId FOrderExtended\n", argc[0]);
        return E_ERR;
    }

    std::string ip     = argc[1];
    std::string port   = argc[2];
    std::string pwd    = argc[3];
    std::string user   = argc[4];

    std::string FOrderId       = argc[5];
    std::string FOrderExtended = argc[6];

    CMyDB mdb;
    mdb.Init(ip, port, pwd, user);

    // test
    //mdb.Query();
    //mdb.Insert();

    mdb.InsertPortalOrder(FOrderId, FOrderExtended);
    mdb.InsertChannelOrder(FOrderId, FOrderExtended);

    cout << "end\n";

}

