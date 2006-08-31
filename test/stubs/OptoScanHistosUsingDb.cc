#include "DQM/SiStripCommissioningClients/test/stubs/OptoScanHistosUsingDb.h"
#include "DataFormats/SiStripCommon/interface/SiStripConstants.h"
#include "DataFormats/SiStripDetId/interface/SiStripControlKey.h"
#include <iostream>

using namespace std;

// -----------------------------------------------------------------------------
/** */
OptoScanHistosUsingDb::OptoScanHistosUsingDb( MonitorUserInterface* mui,
					      string confdb,
					      string partition,
					      uint32_t major,
					      uint32_t minor )
  : OptoScanHistograms( mui ),
    CommissioningHistosUsingDb( confdb, partition, major, minor )
{
  cout << "[" << __PRETTY_FUNCTION__ << "]" << endl;
}

// -----------------------------------------------------------------------------
/** */
OptoScanHistosUsingDb::~OptoScanHistosUsingDb() {
  cout << "[" << __PRETTY_FUNCTION__ << "]" << endl;
}

// -----------------------------------------------------------------------------
/** */
void OptoScanHistosUsingDb::uploadToConfigDb() {
  cout << "[" << __PRETTY_FUNCTION__ << "]" << endl;
  
  // Update LLD descriptions with new bias/gain settings
  db_->resetDeviceDescriptions();
  const SiStripConfigDb::DeviceDescriptions& devices = db_->getDeviceDescriptions( LASERDRIVER );
  update( const_cast<SiStripConfigDb::DeviceDescriptions&>(devices) );
  db_->uploadDeviceDescriptions(false);
  
}

// -----------------------------------------------------------------------------
/** */
void OptoScanHistosUsingDb::update( SiStripConfigDb::DeviceDescriptions& devices ) {
  
  // Iterate through devices and update device descriptions
  SiStripConfigDb::DeviceDescriptions::iterator idevice;
  for ( idevice = devices.begin(); idevice != devices.end(); idevice++ ) {
    
    // Check device type
    if ( (*idevice)->getDeviceType() != LASERDRIVER ) {
      cerr << "[" << __PRETTY_FUNCTION__ << "]"
	   << " Unexpected device type: " << (*idevice)->getDeviceType() << endl;
      continue;
    }
    
    // Iterate through LLD channels
    for ( uint16_t ichan = 0; ichan < sistrip::CHANS_PER_LLD; ichan++ ) {
      
      // Construct key from device description
      uint32_t key = SiStripControlKey::key( sistrip::invalid_,  //@@ FEC crate not used (?)
					     (*idevice)->getFecSlot(),
					     (*idevice)->getRingSlot(),
					     (*idevice)->getCcuAddress(),
					     (*idevice)->getChannel(),
					     ichan );

      // Retrieve description
      laserdriverDescription* desc = reinterpret_cast<laserdriverDescription*>( *idevice );
      if ( desc ) {
	cerr << "[" << __PRETTY_FUNCTION__ << "]"
	     << " Unable to dynamic cast to laserdriverDescription*" << endl;
	continue;
      }
      
      // Iterate through all channels and extract LLD settings 
      map<uint32_t,OptoScanAnalysis::Monitorables>::const_iterator iter = data_.find( key );
      if ( iter != data_.end() ) {

	cout << "[" << __PRETTY_FUNCTION__ << "]"
	     << " Initial bias/gain settings for LLD channel " << ichan << ": " 
	     << desc->getGain(ichan) << "/" << desc->getBias(ichan)
	     << endl;

	uint16_t gain = iter->second.gain_;
	desc->setGain( ichan, gain );
	desc->setBias( ichan, iter->second.bias_[gain] );
	
	cout << "[" << __PRETTY_FUNCTION__ << "]"
	     << " Updated bias/gain settings for LLD channel " << ichan << ": " 
	     << desc->getGain(ichan) << "/" << desc->getBias(ichan)
	     << endl;
      
      } else {
	cerr << "[" << __PRETTY_FUNCTION__ << "]"
	     << " Unable to find PLL settings for device with params FEC/slot/ring/CCU/LLD channel: " 
	     << (*idevice)->getFecSlot() << "/"
	     << (*idevice)->getRingSlot() << "/"
	     << (*idevice)->getCcuAddress() << "/"
	     << (*idevice)->getChannel()
	     << ichan
	     << endl;
      }
      
    }

  }

}








  
