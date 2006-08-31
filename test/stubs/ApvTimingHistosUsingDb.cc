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

  // Check maximum delay calculated by analysis loop
  if ( maxDelay_ < 0. || maxDelay_ > sistrip::maximum_ ) { 
    cerr << "[" << __PRETTY_FUNCTION__ << "]"
	 << " Unexpected value for maximum delay: " << maxDelay_ << endl
	 << " Aborted upload to database..." << endl;
    return; 
  }

  cout << "here 1" << endl;
  
  // Calculate PLL delay for each module
  map<uint32_t,uint32_t> pll_delays;
  pllDelays( pll_delays );
  cout << "here 2" << endl;
  
  //@@ what about fed thresholds ??? laurent does this here! need fed key...
  //@@ have map<int,int> in base constructed using histo name...
  
  // Retrieve descriptions for all PLL devices
  SiStripConfigDb::DeviceDescriptions devices;
  if ( db_ ) { devices = db_->getDeviceDescriptions( PLL ); }
  else { cout << "here 30" << endl; }
  cout << "here 3" << endl;

  // Update delay settings in PLL device descriptions
  update( pll_delays, devices );
  cout << "here 4" << endl;
  
  // Reset local cache 
  db_->resetDeviceDescriptions();
  cout << "here 5" << endl;
  // Write all descriptions to cache
  db_->setDeviceDescriptions( devices ); 
  cout << "here 6" << endl;
  // Upload all descriptions in cache to database (minor version)
  db_->uploadDeviceDescriptions(false);
  cout << "here 7" << endl;

}

// -----------------------------------------------------------------------------
/** */
void ApvTimingHistosUsingDb::pllDelays( map<uint32_t,uint32_t>& pll_delays ) {
  
  // Iterate through all channels and calc PLL delay per module
  map<uint32_t,ApvTimingAnalysis::Monitorables>::const_iterator iter;
  for ( iter = data_.begin(); iter != data_.end(); iter++ ) {
    
    // Check delay and tick height are valid
    if ( iter->second.delay_ > sistrip::maximum_ ) { continue; }
    if ( iter->second.height_ < 100. ) { continue; }
    
    // Create key specific to module
    SiStripControlKey::ControlPath path = SiStripControlKey::path( iter->first );
    //@@ change this to SiStripControlPath
    uint32_t key = SiStripControlKey::key( sistrip::invalid_, //@@ one partition only!!!
					   path.fecSlot_,
					   path.fecRing_,
					   path.ccuAddr_,
					   path.ccuChan_ );
    
    float delay = maxDelay_ - iter->second.delay_;
    pll_delays[key] = static_cast<uint32_t>( rint(delay*24./25.) );
    
  }
  
}

// -----------------------------------------------------------------------------
/** */
void ApvTimingHistosUsingDb::update( const map<uint32_t,uint32_t>& pll_delays,
				     SiStripConfigDb::DeviceDescriptions& devices ) {
  
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
    //@@ change this to SiStripControlPath!!!
    uint32_t key = SiStripControlKey::key( sistrip::invalid_, //@@ one partition only!!!
					   (*idevice)->getFecSlot(),
					   (*idevice)->getRingSlot(),
					   (*idevice)->getCcuAddress(),
					   (*idevice)->getChannel() );

    map<uint32_t,uint32_t>::const_iterator ipll = pll_delays.find( key );
    if ( ipll != pll_delays.end() ) {
      pllDescription* desc = reinterpret_cast<pllDescription*>( *idevice );
      
      cout << "[" << __PRETTY_FUNCTION__ << "]"
	   << " Initial PLL settings (coarse/fine): " 
	   << desc->getDelayCoarse() << "/" << desc->getDelayFine()
	   << endl;
      
      uint32_t coarse = desc->getDelayCoarse() + ( desc->getDelayFine() + ipll->second ) / 24;
      uint32_t fine   = ( desc->getDelayFine() + ipll->second ) % 24;
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





