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

  //@@ what about fed thresholds ??? missing this! need fed key...
  //@@ have map<int,int> in base constructed using histo name...

  // Retrieve descriptions for all PLL devices
  SiStripConfigDb::FedDescriptions feds = db_->getFedDescriptions();

  // Update peds/noise values in FED descriptions
  update( feds );
  
  // Reset local cache 
  db_->resetFedDescriptions();
  // Write all descriptions to cache
  db_->setFedDescriptions( feds ); 
  // Upload all descriptions in cache to database (minor version)
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
      
      map<uint32_t,PedestalsAnalysis::Monitorables>::const_iterator iter = data_.find( fec_key );
      if ( iter != data_.end() ) {

	// Iterate through APVs and strips
	for ( uint16_t iapv = 0; iapv < sistrip::APVS_PER_FEDCH; iapv++ ) {
	  for ( uint16_t istr = 0; istr < iter->second.peds_[iapv].size(); istr++ ) { 

	    static float high_threshold = 5.;
	    static float low_threshold  = 5.;
	    static bool  disable_strip  = false;
	    Fed9U::Fed9UStripDescription data( static_cast<uint32_t>( iter->second.peds_[iapv][istr] ), 
					       high_threshold, 
					       low_threshold, 
					       iter->second.noise_[iapv][istr],
					       disable_strip );
	    Fed9U::Fed9UAddress addr( ichan, iapv, istr );
	    (*ifed)->getFedStrips().setStrip( addr, data );

	  }
	}
      }
    }
  }

}
