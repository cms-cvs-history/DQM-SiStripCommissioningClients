#include "DQM/SiStripCommissioningClients/test/stubs/ApvTimingHistosUsingDb.h"
#include "DataFormats/SiStripCommon/interface/SiStripConstants.h"
#include "DataFormats/SiStripCommon/interface/SiStripFecKey.h"
#include "DataFormats/SiStripCommon/interface/SiStripFedKey.h"
#include <iostream>

using namespace std;

// -----------------------------------------------------------------------------
/** */
ApvTimingHistosUsingDb::ApvTimingHistosUsingDb( MonitorUserInterface* mui,
						const DbParams& params )
  : ApvTimingHistograms( mui ),
    CommissioningHistosUsingDb( params )
{
  cout << endl // LogTrace(mlDqmClient_) 
       << "[ApvTimingHistosUsingDb::" << __func__ << "]"
       << " Constructing object...";
}

// -----------------------------------------------------------------------------
/** */
ApvTimingHistosUsingDb::~ApvTimingHistosUsingDb() {
  cout << endl // LogTrace(mlDqmClient_) 
       << "[ApvTimingHistosUsingDb::" << __func__ << "]"
       << " Destructing object...";
}

// -----------------------------------------------------------------------------
/** */
void ApvTimingHistosUsingDb::uploadToConfigDb() {
  
  if ( !db_ ) {
    cerr << endl // edm::LogWarning(mlDqmClient_) 
	 << "[ApvTimingHistosUsingDb::" << __func__ << "]"
	 << " NULL pointer to SiStripConfigDb interface! Aborting upload...";
    return;
  }
  
  // Update PLL device descriptions
  db_->resetDeviceDescriptions();
  SiStripConfigDb::DeviceDescriptions devices;
  db_->getDeviceDescriptions( devices, PLL ); 
  update( devices );
  db_->uploadDeviceDescriptions(false);
  cout << endl // LogTrace(mlDqmClient_) 
       << "[ApvTimingHistosUsingDb::" << __func__ << "]"
       << "Upload of PLL settings to DB finished!";
  
  // Update FED descriptions with new ticker thresholds
  db_->resetFedDescriptions();
  const SiStripConfigDb::FedDescriptions& feds = db_->getFedDescriptions(); 
  update( const_cast<SiStripConfigDb::FedDescriptions&>(feds) );
  db_->uploadFedDescriptions(false);
  cout << endl // LogTrace(mlDqmClient_) 
       << "[ApvTimingHistosUsingDb::" << __func__ << "]"
       << "Upload of ticker thresholds to DB finished!";
  
}

// -----------------------------------------------------------------------------
/** */
void ApvTimingHistosUsingDb::update( SiStripConfigDb::DeviceDescriptions& devices ) {

  // Iterate through devices and update device descriptions
  SiStripConfigDb::DeviceDescriptions::iterator idevice;
  for ( idevice = devices.begin(); idevice != devices.end(); idevice++ ) {
    
    // Check device type
    if ( (*idevice)->getDeviceType() != PLL ) {
      cerr << endl // edm::LogWarning(mlDqmClient_) 
	   << "[ApvTimingHistosUsingDb::" << __func__ << "]"
	   << " Unexpected device type: " << (*idevice)->getDeviceType();
      continue;
    }
    
    // Cast to retrieve appropriate description object
    pllDescription* desc = dynamic_cast<pllDescription*>( *idevice );
    if ( !desc ) {
      cerr << endl // edm::LogWarning(mlDqmClient_) 
	   << "[ApvTimingHistosUsingDb::" << __func__ << "]"
	   << " Unable to dynamic cast to pllDescription*";
      continue;
    }
    
    // Construct key from device description
    uint32_t key = SiStripFecKey::key( sistrip::invalid_, //@@ one partition only!!!
				       (*idevice)->getFecSlot(),
				       (*idevice)->getRingSlot(),
				       (*idevice)->getCcuAddress(),
				       (*idevice)->getChannel() );
    
    // Locate appropriate analysis object    
    map<uint32_t,ApvTimingAnalysis>::const_iterator iter = data_.find( key );
    if ( iter != data_.end() ) { 
      
      // Check delay value
      if ( iter->second.maxTime() < 0. || iter->second.maxTime() > sistrip::maximum_ ) { 
	cerr << endl // edm::LogWarning(mlDqmClient_) 
	     << "[ApvTimingHistosUsingDb::" << __func__ << "]"
	     << " Unexpected maximum time setting: "
	     << iter->second.maxTime();
	continue;
      }
      
      // Check delay and tick height are valid
      if ( iter->second.delay() < 0. || 
	   iter->second.delay() > sistrip::maximum_ ) { 
	cerr << endl // edm::LogWarning(mlDqmClient_) 
	     << "[ApvTimingHistosUsingDb::" << __func__ << "]"
	     << " Unexpected delay value: "
	     << iter->second.delay();
	continue; 
      }
      if ( iter->second.height() < 100. ) { 
	cerr << endl // edm::LogWarning(mlDqmClient_) 
	     << "[ApvTimingHistosUsingDb::" << __func__ << "]"
	     << " Unexpected tick height: "
	     << iter->second.height();
	continue; 
      }
      
      cout << endl // LogTrace(mlDqmClient_) 
	   << "[ApvTimingHistosUsingDb::" << __func__ << "]"
	   << " Initial PLL settings (coarse/fine): " 
	   << desc->getDelayCoarse() << "/" << desc->getDelayFine();
	
      // Update PLL settings
      uint32_t delay = static_cast<uint32_t>( rint( iter->second.delay() * 24. / 25. ) ); 
      uint32_t coarse = desc->getDelayCoarse() + ( desc->getDelayFine() + delay ) / 24;
      uint32_t fine   = ( desc->getDelayFine() + delay ) % 24;
      desc->setDelayCoarse(coarse);
      desc->setDelayFine(fine);
      
      cout << endl // LogTrace(mlDqmClient_) 
	   << "[ApvTimingHistosUsingDb::" << __func__ << "]"
	   << " Updated PLL settings (coarse/fine): " 
	   << desc->getDelayCoarse() << "/" << desc->getDelayFine();
      
    } else {
      cerr << endl // edm::LogWarning(mlDqmClient_) 
	   << "[ApvTimingHistosUsingDb::" << __func__ << "]"
	   << " Unable to find PLL settings for device with params FEC/slot/ring/CCU: " 
	   << (*idevice)->getFecSlot() << "/"
	   << (*idevice)->getRingSlot() << "/"
	   << (*idevice)->getCcuAddress() << "/"
	   << (*idevice)->getChannel();
    }

  }
  
}

// -----------------------------------------------------------------------------
/** */
void ApvTimingHistosUsingDb::update( SiStripConfigDb::FedDescriptions& feds ) {
  
  // Iterate through feds and update fed descriptions
  SiStripConfigDb::FedDescriptions::iterator ifed;
  for ( ifed = feds.begin(); ifed != feds.end(); ifed++ ) {
    
    for ( uint16_t ichan = 0; ichan < sistrip::FEDCH_PER_FED; ichan++ ) {

      // Retrieve FEC key from FED-FEC map
      uint32_t fec_key = 0;
      uint32_t fed_key = SiStripFedKey::key( static_cast<uint16_t>((*ifed)->getFedId()), ichan );
      FedToFecMap::const_iterator ifec = mapping().find(fed_key);
      if ( ifec != mapping().end() ) { fec_key = ifec->second; }
      else {
	cerr << endl // edm::LogWarning(mlDqmClient_) 
	     << "[ApvTimingHistosUsingDb::" << __func__ << "]"
	     << " Unable to find FEC key for FED id/ch: "
	     << (*ifed)->getFedId() << "/" << ichan;
	continue; //@@ write defaults here?... 
      }
      
      // Locate appropriate analysis object 
      map<uint32_t,ApvTimingAnalysis>::const_iterator iter = data_.find( fec_key );
      if ( iter != data_.end() ) { 
	uint32_t thresh = static_cast<uint32_t>( iter->second.base() + (2./3.)*iter->second.height() );
	Fed9U::Fed9UAddress addr( ichan );
	(*ifed)->setFrameThreshold( addr, thresh );
      } else {
	SiStripFecKey::Path path = SiStripFecKey::path( fec_key );
	cerr << endl // edm::LogWarning(mlDqmClient_) 
	     << "[ApvTimingHistosUsingDb::" << __func__ << "]"
	     << " Unable to find ticker thresholds for FedKey/Id/Ch: " 
	     << hex << setw(8) << setfill('0') << fed_key << dec << "/"
	     << (*ifed)->getFedId() << "/"
	     << ichan
	     << " and device with FEC/slot/ring/CCU/LLDchan: " 
	     << path.fecCrate_ << "/"
	     << path.fecSlot_ << "/"
	     << path.fecRing_ << "/"
	     << path.ccuAddr_ << "/"
	     << path.ccuChan_ << "/"
	     << path.channel_;
      }

    }
  }
  
}
