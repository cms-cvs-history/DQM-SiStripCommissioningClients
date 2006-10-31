#include "DQM/SiStripCommissioningClients/test/stubs/SiStripCommissioningDbClient.h"
#include "DQM/SiStripCommissioningClients/test/stubs/CommissioningHistosUsingDb.h"
#include "DQM/SiStripCommissioningClients/test/stubs/ApvTimingHistosUsingDb.h"
#include "DQM/SiStripCommissioningClients/test/stubs/OptoScanHistosUsingDb.h"
#include "DQM/SiStripCommissioningClients/test/stubs/VpspScanHistosUsingDb.h"
//#include "DQM/SiStripCommissioningClients/test/stubs/FedTimingHistosUsingDb.h"
#include "DQM/SiStripCommissioningClients/test/stubs/FedCablingHistosUsingDb.h"
#include "DQM/SiStripCommissioningClients/test/stubs/PedestalsHistosUsingDb.h"
#include "OnlineDB/SiStripConfigDb/interface/SiStripConfigDb.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "xdata/include/xdata/UnsignedLong.h"
#include "xdata/include/xdata/String.h"
#include <SealBase/Callback.h>
#include <iostream>

XDAQ_INSTANTIATOR_IMPL(SiStripCommissioningDbClient);

using namespace std;
using namespace sistrip;

// -----------------------------------------------------------------------------
/** */
SiStripCommissioningDbClient::SiStripCommissioningDbClient( xdaq::ApplicationStub* stub ) 
  : SiStripCommissioningClient( stub ),
    confdb_(""),
    partition_(""),
    major_(0),
    minor_(0)
{
  cout << endl // LogTrace(mlDqmClient_) 
       << "[SiStripCommissioningDbClient::" << __func__ << "]"
       << " Constructing object...";

  // Retrieve database configuration parameters
  xdata::InfoSpace *sp = getApplicationInfoSpace();
  sp->fireItemAvailable( "confdb", &confdb_ );
  sp->fireItemAvailable( "partition", &partition_ );
  sp->fireItemAvailable( "major", &major_ );
  sp->fireItemAvailable( "minor", &minor_ );
  
}

// -----------------------------------------------------------------------------
/** */
SiStripCommissioningDbClient::~SiStripCommissioningDbClient() {
}

// -----------------------------------------------------------------------------
/** Create histograms for given commissioning task. */
void SiStripCommissioningDbClient::createHistograms( const sistrip::Task& task ) const {

  // Check if object already exists
  if ( histos_ ) { return; }
  
  // Create corresponding "commissioning histograms" object 
  if      ( task == sistrip::APV_TIMING )     { histos_ = new ApvTimingHistosUsingDb( mui_, confdb_.value_, partition_.value_, major_.value_, minor_.value_ ); }
  else if ( task == sistrip::FED_CABLING )    { histos_ = new FedCablingHistosUsingDb( mui_, confdb_.value_, partition_.value_, major_.value_, minor_.value_ ); }
  //else if ( task == sistrip::FED_TIMING )     { histos_ = new FedTimingHistosUsingDb( mui_, confdb_.value_, partition_.value_, major_.value_, minor_.value_ ); }
  else if ( task == sistrip::PEDESTALS )      { histos_ = new PedestalsHistosUsingDb( mui_, confdb_.value_, partition_.value_, major_.value_, minor_.value_ ); }
  else if ( task == sistrip::VPSP_SCAN )      { histos_ = new VpspScanHistosUsingDb( mui_, confdb_.value_, partition_.value_, major_.value_, minor_.value_ ); }
  else if ( task == sistrip::OPTO_SCAN )      { histos_ = new OptoScanHistosUsingDb( mui_, confdb_.value_, partition_.value_, major_.value_, minor_.value_ ); }
  else if ( task == sistrip::UNDEFINED_TASK ) { histos_ = 0; }
  else if ( task == sistrip::UNKNOWN_TASK ) {
    histos_ = 0;
    cerr << endl // edm::LogWarning(mlDqmClient_)
	 << "[SiStripCommissioningDbClient::" << __func__ << "]"
	 << " Unknown commissioning task!";
  }
  
}

// -----------------------------------------------------------------------------
/** */
void SiStripCommissioningDbClient::uploadToConfigDb() {

  if ( !histos_ ) { 
    cerr << endl // edm::LogWarning(mlDqmClient_)
	 << "[SiStripCommissioningDbClient::" << __func__ << "]"
	 << " NULL pointer to CommissioningHistograms!";
    return;
  }
  
  seal::Callback action;
  action = seal::CreateCallback( histos_, 
				 &CommissioningHistograms::uploadToConfigDb
				 ); //@@ no arguments
  
  if ( mui_ ) { 
    cout << endl // LogTrace(mlDqmClient_)
	 << "[SiStripCommissioningDbClient::" << __func__ << "]"
	 << " Scheduling this action...";
    mui_->addCallback(action); 
  } else { 
    cerr << endl // edm::LogWarning(mlDqmClient_)
	 << "[SiStripCommissioningDbClient::" << __func__ << "]"
	 << " NULL pointer to MonitorUserInterface!"; 
    return;
  }
  
}
