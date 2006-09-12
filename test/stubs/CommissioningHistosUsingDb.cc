#include "DQM/SiStripCommissioningClients/test/stubs/CommissioningHistosUsingDb.h"
#include "OnlineDB/SiStripConfigDb/interface/SiStripConfigDb.h"
#include <iostream>

using namespace std;

// -----------------------------------------------------------------------------
/** */
CommissioningHistosUsingDb::CommissioningHistosUsingDb( string confdb,
							string partition,
							uint32_t major,
							uint32_t minor ) 
  : db_(0)
{
  cout << "[" << __PRETTY_FUNCTION__ << "]" << endl;

  // Extract db connections params from CONFDB
  string login = "";
  string passwd = "";
  string path = "";
  uint32_t ipass = confdb.find("/");
  uint32_t ipath = confdb.find("@");
  if ( ( ipass != std::string::npos ) && 
       ( ipath != std::string::npos ) ) {
    login = confdb.substr( 0, ipass );
    passwd = confdb.substr( ipass+1, ipath-ipass-1 );
    path = confdb.substr( ipath+1, confdb.size() );
  }
  
  // Create database interface
  db_ = new SiStripConfigDb( login, passwd, path, partition, major, minor );
  db_->openDbConnection();
  
  cout << "[" << __PRETTY_FUNCTION__ << "]"
       << " SiStripConfigDB ptr: " << db_
       << " confdb: " << confdb
       << " login: " << login
       << " passwd: " << passwd
       << " path: " << path
       << " partition: " << partition
       << " major: " << major
       << " minor: " << minor
       << endl;

}

// -----------------------------------------------------------------------------
/** */
CommissioningHistosUsingDb::~CommissioningHistosUsingDb() {
  cout << "[" << __PRETTY_FUNCTION__ << "]" << endl;
  if ( db_ ) {
    db_->closeDbConnection();
    delete db_;
  }
}
