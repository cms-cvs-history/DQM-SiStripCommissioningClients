#include "DQM/SiStripCommissioningClients/test/stubs/PedestalsHistosUsingDb.h"
#include "DataFormats/SiStripCommon/interface/SiStripConstants.h"
#include "DataFormats/SiStripDetId/interface/SiStripControlKey.h"
#include "DataFormats/SiStripDetId/interface/SiStripReadoutKey.h"
#include <iostream>

using namespace std;

// -----------------------------------------------------------------------------
/** */
PedestalsHistosUsingDb::PedestalsHistosUsingDb( MonitorUserInterface* mui,
					      string confdb,
					      string partition,
					      uint32_t major,
					      uint32_t minor )
  : PedestalsHistograms( mui ),
    CommissioningHistosUsingDb( confdb, partition, major, minor )
{
  cout << "[" << __PRETTY_FUNCTION__ << "]" << endl;
}

// -----------------------------------------------------------------------------
/** */
PedestalsHistosUsingDb::~PedestalsHistosUsingDb() {
  cout << "[" << __PRETTY_FUNCTION__ << "]" << endl;
}

// -----------------------------------------------------------------------------
/** */
void PedestalsHistosUsingDb::uploadToConfigDb() {
  cout << "[" << __PRETTY_FUNCTION__ << "]" << endl;

  if ( !db_ ) {
    cerr << "[" << __PRETTY_FUNCTION__ << "]" 
	 << " NULL pointer to SiStripConfigDb interface! Aborting upload..."
	 << endl;
    return;
  }

  // Update FED descriptions with new peds/noise values
  db_->resetFedDescriptions();
  const SiStripConfigDb::FedDescriptions& devices = db_->getFedDescriptions(); 
  update( const_cast<SiStripConfigDb::FedDescriptions&>(devices) );
  db_->uploadFedDescriptions(false);

}

// -----------------------------------------------------------------------------
/** */
void PedestalsHistosUsingDb::update( SiStripConfigDb::FedDescriptions& feds ) {
 
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
      
      map<uint32_t,PedestalsAnalysis>::const_iterator iter = data_.find( fec_key );
      if ( iter != data_.end() ) {

	// Iterate through APVs and strips
	for ( uint16_t iapv = 0; iapv < sistrip::APVS_PER_FEDCH; iapv++ ) {
	  for ( uint16_t istr = 0; istr < iter->second.peds()[iapv].size(); istr++ ) { 

	    static float high_threshold = 5.;
	    static float low_threshold  = 5.;
	    static bool  disable_strip  = false;
	    Fed9U::Fed9UStripDescription data( static_cast<uint32_t>( iter->second.peds()[iapv][istr] ), 
					       high_threshold, 
					       low_threshold, 
					       iter->second.noise()[iapv][istr],
					       disable_strip );
	    Fed9U::Fed9UAddress addr( ichan, iapv, istr );
	    (*ifed)->getFedStrips().setStrip( addr, data );

	  }
	}
      
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