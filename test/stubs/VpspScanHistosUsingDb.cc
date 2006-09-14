#include "DQM/SiStripCommissioningClients/test/stubs/VpspScanHistosUsingDb.h"
#include "DataFormats/SiStripCommon/interface/SiStripConstants.h"
#include "DataFormats/SiStripDetId/interface/SiStripControlKey.h"
#include <iostream>

using namespace std;

// -----------------------------------------------------------------------------
/** */
VpspScanHistosUsingDb::VpspScanHistosUsingDb( MonitorUserInterface* mui,
					      string confdb,
					      string partition,
					      uint32_t major,
					      uint32_t minor )
  : VpspScanHistograms( mui ),
    CommissioningHistosUsingDb( confdb, partition, major, minor )
{
  cout << "[" << __PRETTY_FUNCTION__ << "]" << endl;
}

// -----------------------------------------------------------------------------
/** */
VpspScanHistosUsingDb::~VpspScanHistosUsingDb() {
  cout << "[" << __PRETTY_FUNCTION__ << "]" << endl;
}

// -----------------------------------------------------------------------------
/** */
void VpspScanHistosUsingDb::uploadToConfigDb() {
  cout << "[" << __PRETTY_FUNCTION__ << "]" << endl;
  
  // Update all APV device descriptions with new VPSP settings
  db_->resetDeviceDescriptions();
  const SiStripConfigDb::DeviceDescriptions& devices = db_->getDeviceDescriptions( APV25 );
  update( const_cast<SiStripConfigDb::DeviceDescriptions&>(devices) );
  db_->uploadDeviceDescriptions(false);
  cout << "[" << __PRETTY_FUNCTION__ << "] Upload of VPSP settings to DB finished!" << endl;

}

// -----------------------------------------------------------------------------
/** */
void VpspScanHistosUsingDb::update( SiStripConfigDb::DeviceDescriptions& devices ) {
  
  // Iterate through devices and update device descriptions
  SiStripConfigDb::DeviceDescriptions::iterator idevice;
  for ( idevice = devices.begin(); idevice != devices.end(); idevice++ ) {
    
    // Check device type
    if ( (*idevice)->getDeviceType() != APV25 ) {
      cerr << "[" << __PRETTY_FUNCTION__ << "]"
	   << " Unexpected device type: " << (*idevice)->getDeviceType() << endl;
      continue;
    }
    
    // Retrieve description
    apvDescription* desc = reinterpret_cast<apvDescription*>( *idevice );
    if ( desc ) {
      cerr << "[" << __PRETTY_FUNCTION__ << "]"
	   << " Unable to dynamic cast to apvDescription*" << endl;
      continue;
    }
    
    // Retrieve LLD channel and APV numbers
    uint16_t ichan = ( desc->getAddress() - 0x20 ) / 2;
    uint16_t iapv  = ( desc->getAddress() - 0x20 ) % 2;

    // Construct key from device description
    uint32_t key = SiStripControlKey::key( sistrip::invalid_,  //@@ FEC crate not used (?)
					   (*idevice)->getFecSlot(),
					   (*idevice)->getRingSlot(),
					   (*idevice)->getCcuAddress(),
					   (*idevice)->getChannel(),
					   ichan );
      
    // Iterate through all channels and extract LLD settings 
    map<uint32_t,VpspScanAnalysis>::const_iterator iter = data_.find( key );
    if ( iter != data_.end() ) {
      
      cout << "[" << __PRETTY_FUNCTION__ << "]"
	   << " Initial VPSP setting: " << desc->getVpsp() << endl;
      
      if ( iapv == 0 ) { desc->setVpsp( iter->second.vpsp0() ); }
      if ( iapv == 1 ) { desc->setVpsp( iter->second.vpsp1() ); }
      
      cout << "[" << __PRETTY_FUNCTION__ << "]"
	   << " Updated VPSP setting: " << desc->getVpsp() << endl;
      
    } else {
      cerr << "[" << __PRETTY_FUNCTION__ << "]"
	   << " Unable to find PLL settings for device with params FEC/slot/ring/CCU/LLD channel/APV number: " 
	   << (*idevice)->getFecSlot() << "/"
	   << (*idevice)->getRingSlot() << "/"
	   << (*idevice)->getCcuAddress() << "/"
	   << (*idevice)->getChannel()
	   << ichan << "/" << iapv
	   << endl;
    }
      
  }

}








  

