#include "DQM/SiStripCommissioningClients/test/stubs/ApvTimingHistosUsingDb.h"
#include "DataFormats/SiStripCommon/interface/SiStripConstants.h"
#include "DataFormats/SiStripDetId/interface/SiStripControlKey.h"
#include <iostream>

using namespace std;

// -----------------------------------------------------------------------------
/** */
ApvTimingHistosUsingDb::ApvTimingHistosUsingDb( MonitorUserInterface* mui,
						string confdb,
						string partition,
						uint32_t major,
						uint32_t minor )
  : ApvTimingHistograms( mui ),
    CommissioningHistosUsingDb( confdb, partition, major, minor )
{
  cout << "[" << __PRETTY_FUNCTION__ << "]" << endl;
}

// -----------------------------------------------------------------------------
/** */
ApvTimingHistosUsingDb::~ApvTimingHistosUsingDb() {
  cout << "[" << __PRETTY_FUNCTION__ << "]" << endl;
}

// -----------------------------------------------------------------------------
/** */
void ApvTimingHistosUsingDb::uploadToConfigDb() {
  cout << "[" << __PRETTY_FUNCTION__ << "]" << endl;
  
  if ( !db_ ) {
    cerr << "[" << __PRETTY_FUNCTION__ << "]" 
	 << " NULL pointer to SiStripConfigDb interface! Aborting upload..."
	 << endl;
    return;
  }
  
  // Update PLL device descriptions
  db_->resetDeviceDescriptions();
  const SiStripConfigDb::DeviceDescriptions& devices = db_->getDeviceDescriptions( PLL ); 
  update( const_cast<SiStripConfigDb::DeviceDescriptions&>(devices) );
  db_->uploadDeviceDescriptions(false);
  cout << "[" << __PRETTY_FUNCTION__ << "] Upload of PLL settings to DB finished!" << endl;
  
  // Update FED descriptions with new ticker thresholds
  db_->resetFedDescriptions();
  const SiStripConfigDb::FedDescriptions& feds = db_->getFedDescriptions(); 
  update( const_cast<SiStripConfigDb::FedDescriptions&>(feds) );
  //db_->uploadFedDescriptions(false); //@@ not working!!! needs to be checked
  //cout << "[" << __PRETTY_FUNCTION__ << "] Upload of ticker thresholds to DB finished!" << endl;
  
}

// -----------------------------------------------------------------------------
/** */
void ApvTimingHistosUsingDb::update( SiStripConfigDb::DeviceDescriptions& devices ) {

  // Iterate through devices and update device descriptions
  SiStripConfigDb::DeviceDescriptions::iterator idevice;
  for ( idevice = devices.begin(); idevice != devices.end(); idevice++ ) {
    
    // Check device type
    if ( (*idevice)->getDeviceType() != PLL ) {
      cerr << "[" << __PRETTY_FUNCTION__ << "]"
	   << " Unexpected device type: " << (*idevice)->getDeviceType() << endl;
      continue;
    }

    // Retrieve description
    pllDescription* desc = reinterpret_cast<pllDescription*>( *idevice );
    if ( desc ) {
      cerr << "[" << __PRETTY_FUNCTION__ << "]"
	   << " Unable to dynamic cast to pllDescription*" << endl;
      continue;
    }
    
    // Construct key from device description
    uint32_t key = SiStripControlKey::key( sistrip::invalid_, //@@ one partition only!!!
					   (*idevice)->getFecSlot(),
					   (*idevice)->getRingSlot(),
					   (*idevice)->getCcuAddress(),
					   (*idevice)->getChannel() );

    // Locate appropriate analysis object    
    map<uint32_t,ApvTimingAnalysis>::const_iterator iter = data_.find( key );
    if ( iter != data_.end() ) { 
      
      // Check delay value
      if ( iter->second.maxTime() < 0. || iter->second.maxTime() > sistrip::maximum_ ) { 
	cerr << "[" << __PRETTY_FUNCTION__ << "]"
	     << " Unexpected maximum time setting: "
	     << iter->second.maxTime() << endl;
	continue;
      }
      
      // Check delay and tick height are valid
      if ( iter->second.delay() < 0. || 
	   iter->second.delay() > sistrip::maximum_ ) { 
	cerr << "[" << __PRETTY_FUNCTION__ << "]"
	     << " Unexpected delay value: "
	     << iter->second.delay() << endl;
	continue; 
      }
      if ( iter->second.height() < 100. ) { 
	cerr << "[" << __PRETTY_FUNCTION__ << "]"
	     << " Unexpected tick height: "
	     << iter->second.height() << endl;
	continue; 
      }
      
      // Cast to retrieve appropriate description object
      pllDescription* desc = reinterpret_cast<pllDescription*>( *idevice );
      if ( !desc ) {
	cout << "[" << __PRETTY_FUNCTION__ << "]"
	     << " NULL pointer to pllDescription!" << endl;
	continue;
      }
      
      cout << "[" << __PRETTY_FUNCTION__ << "]"
	   << " Initial PLL settings (coarse/fine): " 
	   << desc->getDelayCoarse() << "/" << desc->getDelayFine()
	   << endl;
      
      // Update PLL settings
      uint32_t delay = static_cast<uint32_t>( rint( iter->second.delay() * 24. / 25. ) ); 
      uint32_t coarse = desc->getDelayCoarse() + ( desc->getDelayFine() + delay ) / 24;
      uint32_t fine   = ( desc->getDelayFine() + delay ) % 24;
      desc->setDelayCoarse(coarse);
      desc->setDelayFine(fine);
      
      cout << "[" << __PRETTY_FUNCTION__ << "]"
	   << " Updated PLL settings (coarse/fine): " 
	   << desc->getDelayCoarse() << "/" << desc->getDelayFine()
	   << endl;
      
    } else {
      cerr << "[" << __PRETTY_FUNCTION__ << "]"
	   << " Unable to find PLL settings for device with params FEC/slot/ring/CCU: " 
	   << (*idevice)->getFecSlot() << "/"
	   << (*idevice)->getRingSlot() << "/"
	   << (*idevice)->getCcuAddress() << "/"
	   << (*idevice)->getChannel()
	   << endl;
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
      cout << "fed id: " << (*ifed)->getFedId() << endl;
      cout << "fed hw id: " << (*ifed)->getFedHardwareId() << endl;
      uint32_t fed_key = SiStripReadoutKey::key( static_cast<uint16_t>((*ifed)->getFedId()), ichan );
      FedToFecMap::const_iterator ifec = mapping().find(fed_key);
      if ( ifec != mapping().end() ) { fec_key = ifec->second; }
      else {
	cerr << "[" << __PRETTY_FUNCTION__ << "]"
	     << " Unable to find FEC key for FED id/ch: "
	     << (*ifed)->getFedId() << "/" << ichan << endl;
	continue; //@@ write defaults here?... 
      }
      
      // Locate appropriate analysis object    
      map<uint32_t,ApvTimingAnalysis>::const_iterator iter = data_.find( fec_key );
      if ( iter != data_.end() ) { 
	uint32_t thresh = static_cast<uint32_t>( iter->second.base() + (2./3.)*iter->second.height() );
	Fed9U::Fed9UAddress addr( ichan );
	(*ifed)->setFrameThreshold( addr, thresh );
      } else {
	SiStripControlKey::ControlPath path = SiStripControlKey::path( fec_key );
	cerr << "[" << __PRETTY_FUNCTION__ << "]"
	     << " Unable to find ticker thresholds for FED id/ch: " 
	     << (*ifed)->getFedId() << "/"
	     << ichan << "/"
	     << " and device with at FEC/slot/ring/CCU/LLD channel: " 
	     << path.fecCrate_ << "/"
	     << path.fecSlot_ << "/"
	     << path.fecRing_ << "/"
	     << path.ccuAddr_ << "/"
	     << path.ccuChan_ << "/"
	     << path.channel_
	     << endl;
      }
    }
  }
  
}





