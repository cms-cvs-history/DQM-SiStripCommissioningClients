#include "DQM/SiStripCommissioningClients/test/stubs/CommissioningHistosUsingDb.h"
#include "OnlineDB/SiStripConfigDb/interface/SiStripConfigDb.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <iostream>

using namespace std;
using namespace sistrip;

// -----------------------------------------------------------------------------
/** */
CommissioningHistosUsingDb::CommissioningHistosUsingDb( string confdb,
							string partition,
							uint32_t major,
							uint32_t minor ) 
  : db_(0)
{
  cout << endl // LogTrace(mlDqmClient_) 
       << "[CommissioningHistosUsingDb::" << __func__ << "]"
       << " Constructing object...";

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
  if ( login != "" && passwd != "" && path != "" && partition != "" ) {
    db_ = new SiStripConfigDb( login, passwd, path, partition, major, minor );
    db_->openDbConnection();
  } else {
    cerr << endl // edm::LogWarning(mlDqmClient_) 
	 << "[CommissioningHistosUsingDb::" << __func__ << "]"
	 << " Unexpected value for database connection parameters!"
	 << " confdb=" << confdb
	 << " login/passwd@path=" << login << "/" << passwd << "@" << path
	 << " partition=" << partition;
  }
  
  cout << endl // edm::LogWarning(mlDqmClient_) 
       << "[CommissioningHistosUsingDb::" << __func__ << "]"
       << " SiStripConfigDB ptr: " << db_
       << " confdb: " << confdb
       << " login: " << login
       << " passwd: " << passwd
       << " path: " << path
       << " partition: " << partition
       << " major: " << major
       << " minor: " << minor;

}

// -----------------------------------------------------------------------------
/** */
CommissioningHistosUsingDb::~CommissioningHistosUsingDb() {
  if ( db_ ) {
    db_->closeDbConnection();
    delete db_;
  }
  cout << endl // LogTrace(mlDqmClient_) 
       << "[CommissioningHistosUsingDb::" << __func__ << "]"
       << " Destructing object...";
}
