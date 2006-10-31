#include "DQM/SiStripCommissioningClients/test/stubs/OptoScanHistosUsingDb.h"
#include "DataFormats/SiStripCommon/interface/SiStripConstants.h"
#include "DataFormats/SiStripCommon/interface/SiStripFecKey.h"
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
  cout << endl // LogTrace(mlDqmClient_) 
       << "[OptoScanHistosUsingDb::" << __func__ << "]"
       << " Constructing object...";
}

// -----------------------------------------------------------------------------
/** */
OptoScanHistosUsingDb::~OptoScanHistosUsingDb() {
  cout << endl // LogTrace(mlDqmClient_) 
       << "[OptoScanHistosUsingDb::" << __func__ << "]"
       << " Destructing object...";
}

// -----------------------------------------------------------------------------
/** */
void OptoScanHistosUsingDb::uploadToConfigDb() {
  
  if ( !db_ ) {
    cerr << endl // edm::LogWarning(mlDqmClient_) 
	 << "[OptoScanHistosUsingDb::" << __func__ << "]"
	 << " NULL pointer to SiStripConfigDb interface! Aborting upload...";
    return;
  }

  // Update LLD descriptions with new bias/gain settings
  db_->resetDeviceDescriptions();
  SiStripConfigDb::DeviceDescriptions devices;
  db_->getDeviceDescriptions( devices, LASERDRIVER );
  update( devices );
  db_->uploadDeviceDescriptions(false);
  cout << endl // LogTrace(mlDqmClient_) 
       << "[OptoScanHistosUsingDb::" << __func__ << "]"
       << "Upload of LLD settings to DB finished!";

}

// -----------------------------------------------------------------------------
/** */
void OptoScanHistosUsingDb::update( SiStripConfigDb::DeviceDescriptions& devices ) {
  
  // Iterate through devices and update device descriptions
  SiStripConfigDb::DeviceDescriptions::iterator idevice;
  for ( idevice = devices.begin(); idevice != devices.end(); idevice++ ) {
    
    // Check device type
    if ( (*idevice)->getDeviceType() != LASERDRIVER ) {
      cerr << endl // edm::LogWarning(mlDqmClient_) 
	   << "[OptoScanHistosUsingDb::" << __func__ << "]"
	   << " Unexpected device type: " << (*idevice)->getDeviceType();
      continue;
    }
    
    // Iterate through LLD channels
    for ( uint16_t ichan = 0; ichan < sistrip::CHANS_PER_LLD; ichan++ ) {
      
      // Construct key from device description
      uint32_t key = SiStripFecKey::key( sistrip::invalid_,  //@@ FEC crate not used (?)
					 (*idevice)->getFecSlot(),
					 (*idevice)->getRingSlot(),
					 (*idevice)->getCcuAddress(),
					 (*idevice)->getChannel(),
					 ichan );

      // Retrieve description
      laserdriverDescription* desc = reinterpret_cast<laserdriverDescription*>( *idevice );
      if ( desc ) {
	cerr << endl // edm::LogWarning(mlDqmClient_) 
	     << "[OptoScanHistosUsingDb::" << __func__ << "]"
	     << " Unable to dynamic cast to laserdriverDescription*";
	continue;
      }
      
      // Iterate through all channels and extract LLD settings 
      map<uint32_t,OptoScanAnalysis>::const_iterator iter = data_.find( key );
      if ( iter != data_.end() ) {

	cout << endl // LogTrace(mlDqmClient_) 
	     << "[OptoScanHistosUsingDb::" << __func__ << "]"
	     << " Initial bias/gain settings for LLD channel " << ichan << ": " 
	     << desc->getGain(ichan) << "/" << desc->getBias(ichan);

	uint16_t gain = iter->second.gain();
	desc->setGain( ichan, gain );
	desc->setBias( ichan, iter->second.bias()[gain] );
	
	cout << endl // LogTrace(mlDqmClient_) 
	     << "[OptoScanHistosUsingDb::" << __func__ << "]"
	     << " Updated bias/gain settings for LLD channel " << ichan << ": " 
	     << desc->getGain(ichan) << "/" << desc->getBias(ichan);
      
      } else {
	cerr << endl // edm::LogWarning(mlDqmClient_) 
	     << "[OptoScanHistosUsingDb::" << __func__ << "]"
	     << " Unable to find PLL settings for device with params FEC/slot/ring/CCU/LLD channel: " 
	     << (*idevice)->getFecSlot() << "/"
	     << (*idevice)->getRingSlot() << "/"
	     << (*idevice)->getCcuAddress() << "/"
	     << (*idevice)->getChannel()
	     << ichan;
      }
      
    }

  }

}








  
