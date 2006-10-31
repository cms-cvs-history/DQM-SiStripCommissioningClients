#include "DQM/SiStripCommissioningClients/test/stubs/FedCablingHistosUsingDb.h"
#include "DataFormats/SiStripCommon/interface/SiStripConstants.h"
#include "DataFormats/SiStripCommon/interface/SiStripFecKey.h"
#include <iostream>

using namespace std;

// -----------------------------------------------------------------------------
/** */
FedCablingHistosUsingDb::FedCablingHistosUsingDb( MonitorUserInterface* mui,
						  string confdb,
						  string partition,
						  uint32_t major,
						  uint32_t minor )
  : FedCablingHistograms( mui ),
    CommissioningHistosUsingDb( confdb, partition, major, minor )
{
  cout << endl // LogTrace(mlDqmClient_)
       << "[FedCablingHistosUsingDb::" << __func__ << "]"
       << " Constructing object...";
}

// -----------------------------------------------------------------------------
/** */
FedCablingHistosUsingDb::~FedCablingHistosUsingDb() {
  cout << endl // LogTrace(mlDqmClient_)
       << "[FedCablingHistosUsingDb::" << __func__ << "]"
       << " Destructing object...";
}

// -----------------------------------------------------------------------------
/** */
void FedCablingHistosUsingDb::uploadToConfigDb() {
  
  if ( !db_ ) {
    cerr << endl // edm::LogWarning(mlDqmClient_)
	 << "[FedCablingHistosUsingDb::" << __func__ << "]"
	 << " NULL pointer to SiStripConfigDb interface! Aborting upload...";
    return;
  }
  
  // Update FED connection descriptions
  db_->resetFedConnections();
  const SiStripConfigDb::FedConnections& conns = db_->getFedConnections(); 
  update( const_cast<SiStripConfigDb::FedConnections&>(conns) );
  db_->uploadFedConnections(false);
  cout << endl // LogTrace(mlDqmClient_)
       << "[FedCablingHistosUsingDb::" << __func__ << "]"
       << "Upload of FED-FEC connections to DB finished!";

}

// -----------------------------------------------------------------------------
/** */
void FedCablingHistosUsingDb::update( SiStripConfigDb::FedConnections& conns ) {

  if ( !conns.empty() ) {

    // If connections already exist, update
    SiStripConfigDb::FedConnections::iterator iconn;
    for ( iconn = conns.begin(); iconn != conns.end(); iconn++ ) {
      uint32_t key = SiStripFecKey::key( sistrip::invalid_, //@@ one partition only!!!
					 (*iconn)->getSlot(),
					 (*iconn)->getRing(),
					 (*iconn)->getCcu(),
					 (*iconn)->getI2c(),
					 ((*iconn)->getApv()-32)/2 );
      map<uint32_t,FedCablingAnalysis>::const_iterator iter = data_.find( key );
      if ( iter != data_.end() ) { 
	(*iconn)->setFedId( iter->second.fedId() );
	(*iconn)->setFedChannel( iter->second.fedCh() );
      } else {
	cerr << endl // edm::LogWarning(mlDqmClient_)
	     << "[FedCablingHistosUsingDb::" << __func__ << "]"
	     << " Unable to find FED connections info for device with params FEC/slot/ring/CCU: " 
	     << (*iconn)->getSlot() << "/"
	     << (*iconn)->getRing() << "/"
	     << (*iconn)->getCcu() << "/"
	     << (*iconn)->getI2c() << "/"
	     << ((*iconn)->getApv()-32)/2;
      }
    } 
    
  } else {
    cout << endl // LogTrace(mlDqmClient_)
	 << "[FedCablingHistosUsingDb::" << __func__ << "]"
	 << " NO CONNECTIONS OBJECTS! WHAT NOW???";
  }
  
}
