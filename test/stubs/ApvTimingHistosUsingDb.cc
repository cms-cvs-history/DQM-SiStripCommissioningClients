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

  // Check maximum delay calculated by analysis loop
  if ( maxDelay_ < 0. || maxDelay_ > sistrip::maximum_ ) { 
    cerr << "[" << __PRETTY_FUNCTION__ << "]"
	 << " Unexpected value for maximum delay: " << maxDelay_ << endl
	 << " Aborting upload..." << endl;
    return; 
  }

  // Calculate PLL delays and ticker thresholds for each channel
  map<uint32_t,uint32_t> pll_delays;
  map<uint32_t,uint32_t> ticker_thresh;
  settings( pll_delays, ticker_thresh );
  
  // Update PLL device descriptions
  db_->resetDeviceDescriptions();
  const SiStripConfigDb::DeviceDescriptions& devices = db_->getDeviceDescriptions( PLL ); 
  update( pll_delays, const_cast<SiStripConfigDb::DeviceDescriptions&>(devices) );
  db_->uploadDeviceDescriptions(false);
  
  // Update FED descriptions with new ticker thresholds
  if ( true ) {
    db_->resetFedDescriptions();
    const SiStripConfigDb::FedDescriptions& devices = db_->getFedDescriptions(); 
    update( ticker_thresh, const_cast<SiStripConfigDb::FedDescriptions&>(devices) );
    db_->uploadFedDescriptions(false);
  }
  
}

// -----------------------------------------------------------------------------
/** */
void ApvTimingHistosUsingDb::settings( map<uint32_t,uint32_t>& pll_delays,
				       map<uint32_t,uint32_t>& ticker_thresh ) {

  pll_delays.clear();
  ticker_thresh.clear();

  // Iterate through all channels and calc PLL delay per module
  map<uint32_t,ApvTimingAnalysis>::const_iterator iter;
  for ( iter = data_.begin(); iter != data_.end(); iter++ ) {
    
    // Check delay and tick height are valid
    if ( iter->second.delay() > sistrip::maximum_ ) { continue; }
    if ( iter->second.height() < 100. ) { continue; }
    
    // Create key specific to module
    SiStripControlKey::ControlPath path = SiStripControlKey::path( iter->first );
    //@@ change this to SiStripControlPath
    uint32_t key = SiStripControlKey::key( sistrip::invalid_, //@@ one partition only!!!
					   path.fecSlot_,
					   path.fecRing_,
					   path.ccuAddr_,
					   path.ccuChan_ );
    
    float delay = maxDelay_ - iter->second.delay();
    pll_delays[key] = static_cast<uint32_t>( rint(delay*24./25.) );
    ticker_thresh[key] = static_cast<uint32_t>( iter->second.base() + (2./3.)*iter->second.height() );
    
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

// -----------------------------------------------------------------------------
/** */
void ApvTimingHistosUsingDb::update( const map<uint32_t,uint32_t>& ticker_thresh,
				     SiStripConfigDb::FedDescriptions& feds ) {
  
  // Iterate through feds and update fed descriptions
  SiStripConfigDb::FedDescriptions::iterator ifed;
  for ( ifed = feds.begin(); ifed != feds.end(); ifed++ ) {
    
    for ( uint16_t ichan = 0; ichan < sistrip::FEDCH_PER_FED; ichan++ ) {

      // Retrieve FEC key from FED-FEC map
      uint32_t fec_key = 0;
      uint32_t fed_key = SiStripReadoutKey::key( static_cast<uint16_t>((*ifed)->getFedId()), ichan );
      FedToFecMap::const_iterator ifec = mapping().find(fed_key);
      if ( ifec != mapping().end() ) { fec_key = ifec->second; }
      else {
	cerr << "[" << __PRETTY_FUNCTION__ << "]"
	     << " Unable to find FEC key for FED id/ch: "
	     << (*ifed)->getFedId() << "/" << ichan << endl;
	continue; //@@ write defaults here?... 
      }
      
      map<uint32_t,uint32_t>::const_iterator iter = ticker_thresh.find( fec_key );
      if ( iter != ticker_thresh.end() ) {
	Fed9U::Fed9UAddress addr( ichan );
	(*ifed)->setFrameThreshold( addr, iter->second );
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





