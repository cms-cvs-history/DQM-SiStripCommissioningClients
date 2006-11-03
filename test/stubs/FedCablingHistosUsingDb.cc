#include "DQM/SiStripCommissioningClients/test/stubs/FedCablingHistosUsingDb.h"
#include "DataFormats/SiStripCommon/interface/SiStripConstants.h"
#include "DataFormats/SiStripCommon/interface/SiStripFecKey.h"
#include <iostream>

using namespace std;

// -----------------------------------------------------------------------------
/** */
FedCablingHistosUsingDb::FedCablingHistosUsingDb( MonitorUserInterface* mui,
						  const DbParams& params )
  : FedCablingHistograms( mui ),
    CommissioningHistosUsingDb( params )
{
  cout << endl // LogTrace(mlDqmClient_)
       << "[FedCablingHistosUsingDb::" << __func__ << "]"
       << " Constructing object...";
}

// -----------------------------------------------------------------------------
/** */
FedCablingHistosUsingDb::~FedCablingHistosUsingDb() {
  cout << endl // LogTrace(mlDqmClient_)
       << "[FedCablingHistosUsingDb::" << __func__ << "]"
       << " Destructing object...";
}

// -----------------------------------------------------------------------------
/** */
void FedCablingHistosUsingDb::uploadToConfigDb() {
  
  if ( !db_ ) {
    cerr << endl // edm::LogWarning(mlDqmClient_)
	 << "[FedCablingHistosUsingDb::" << __func__ << "]"
	 << " NULL pointer to SiStripConfigDb interface! Aborting upload...";
    return;
  }

  // Retrieve descriptions for DCU id and DetId 
  SiStripConfigDb::DeviceDescriptions dcus; 
  db_->getDeviceDescriptions( dcus, DCU ); 
  SiStripConfigDb::DcuDetIdMap detids;
  detids= db_->getDcuDetIdMap(); 

  // Update FED connection descriptions
  db_->resetFedConnections();
  const SiStripConfigDb::FedConnections& conns = db_->getFedConnections(); 
  update( const_cast<SiStripConfigDb::FedConnections&>(conns), dcus, detids );
  db_->uploadFedConnections(false);
  cout << endl // LogTrace(mlDqmClient_)
       << "[FedCablingHistosUsingDb::" << __func__ << "]"
       << " Completed database upload of " << conns.size() 
       << " FedChannelConnectionDescriptions!";

  // Update FED descriptions with enabled/disabled channels
  db_->resetFedDescriptions();
  const SiStripConfigDb::FedDescriptions& feds = db_->getFedDescriptions(); 
  update( const_cast<SiStripConfigDb::FedDescriptions&>(feds) );
  db_->uploadFedDescriptions(false);
  cout << endl // LogTrace(mlDqmClient_) 
       << "[FedCablingHistosUsingDb::" << __func__ << "]"
       << " Completed database upload of " << feds.size()
       << " Fed9UDescriptions (with connected channels enabled)!";
  
}

// -----------------------------------------------------------------------------
/** */
void FedCablingHistosUsingDb::update( SiStripConfigDb::FedConnections& conns,
				      const SiStripConfigDb::DeviceDescriptions& dcus , 
				      const SiStripConfigDb::DcuDetIdMap& detids ) {

  // Retrieve, clear and update FED-FEC mapping in base class
  FedToFecMap& fed_map = const_cast<FedToFecMap&>( mapping() );
  fed_map.clear();
    
  map<uint32_t,FedCablingAnalysis>::const_iterator ianal;
  for ( ianal = data_.begin(); ianal != data_.end(); ianal++ ) {
    
    // Generate keys
    uint32_t fed_key = SiStripFedKey::key( ianal->second.fedId(), 
					   ianal->second.fedCh() );
    SiStripFedKey::Path fed_path = SiStripFedKey::path( fed_key );
    SiStripFecKey::Path fec_path = SiStripFecKey::path( ianal->first );

    // Pointer to connection description
    FedChannelConnectionDescription* conn = 0;
      
    // Check if connection already exists
    SiStripConfigDb::FedConnections::iterator iconn;
    for ( iconn = conns.begin(); iconn != conns.end(); iconn++ ) {
      if ( *iconn ) {
	if ( (*iconn)->getFecInstance() == fec_path.fecCrate_ &&
	     (*iconn)->getSlot() == fec_path.fecSlot_ &&
	     (*iconn)->getRing() == fec_path.fecRing_ &&
	     (*iconn)->getCcu() == fec_path.ccuAddr_ &&
	     (*iconn)->getI2c() == fec_path.ccuChan_ &&
	     (*iconn)->getApv() == static_cast<uint16_t>(32+2*fec_path.channel_) ) {
	  conn = *iconn;
	}
      } else {
	cerr << endl // edm::LogWarning(mlDqmClient_)
	     << "[FedCablingHistosUsingDb::" << __func__ << "]"
	     << " Vector contains NULL pointer to FedChannelConnectionDescription!";
      }
    }
    
    // If connection does not already exist
    if ( !conn ) { conn = new FedChannelConnectionDescription(); }
    else {
      uint16_t ichan = sistrip::invalid_;
      if ( conn->getApv() ) { ichan = static_cast<uint16_t>((conn->getApv()-32)/2); }
      cerr << endl // edm::LogWarning(mlDqmClient_)
	   << "[FedCablingHistosUsingDb::" << __func__ << "]"
	   << " FedChannelConnectionDescription already exists for"
	   << " Crate/FEC/Ring/CCU/Module/LLDchan: "	
	   << conn->getFecInstance() << "/"
	   << conn->getSlot() << "/"
	   << conn->getRing() << "/"
	   << conn->getCcu() << "/"
	   << ichan
	   << "! Updating FedId/Ch from "
	   << conn->getFedId() << "/"
	   << conn->getFedChannel() << "/"
	   << " to "
	   << ianal->second.fedId() << "/"
	   << ianal->second.fedCh();
    }

    // Set data members
    conn->setFedId( ianal->second.fedId() );
    conn->setFedChannel( ianal->second.fedCh() );
    conn->setFecInstance( fec_path.fecCrate_ );
    conn->setSlot( fec_path.fecSlot_ );
    conn->setRing( fec_path.fecRing_ );
    conn->setCcu( fec_path.ccuAddr_ );
    conn->setI2c( fec_path.ccuChan_ );
    conn->setApv( 32+2*fec_path.channel_ );
    conn->setDcuHardId( sistrip::invalid_ );
    conn->setDetId( sistrip::invalid_ );
    conn->setFiberLength( sistrip::invalid_ );
    conn->setApvPairs( sistrip::invalid_ );

    // Retrieve DCU id 
    SiStripConfigDb::DeviceDescriptions::const_iterator idcu;
    for ( idcu = dcus.begin(); idcu != dcus.end(); idcu++ ) {
      dcuDescription* dcu = dynamic_cast<dcuDescription*>( *idcu );
      keyType key = dcu->getKey(); 
      SiStripFecKey::Path path( fec_path.fecCrate_, //@@ sistrip::invalid_ ???
				getFecKey(key),
				getRingKey(key),
				getCcuKey(key),
				getChannelKey(key) );
      if ( path.fecCrate_ == fec_path.fecCrate_ && 
	   path.fecRing_ == fec_path.fecRing_ && 
	   path.fecSlot_ == fec_path.fecSlot_ && 
	   path.ccuAddr_ == fec_path.ccuAddr_ && 
	   path.ccuChan_ == fec_path.ccuChan_ ) {
	if ( dcu ) { conn->setDcuHardId( dcu->getDcuHardId() ); }
      }
    }

    // Retrieve DetId
    SiStripConfigDb::DcuDetIdMap::const_iterator idet = detids.find( conn->getDcuHardId() );
    if ( idet != detids.end() ) { 
      conn->setDetId( idet->second->getDetId() );
      conn->setApvPairs( idet->second->getApvNumber()/2 );
      conn->setFiberLength( static_cast<uint32_t>( idet->second->getFibreLength() ) );
    }
      
    // Add FedChannelConnectionDescription to vector
    conns.push_back(conn);
      
    // Add entry to FED-FEC mapping object if FedKey is valid
    if ( fed_path.fedId_ != sistrip::invalid_ || 
	 fed_path.fedCh_ != sistrip::invalid_ ) {
      fed_map[fed_key] = ianal->first;
    } else {
      cerr << endl // edm::LogWarning(mlDqmClient_)
	   << "[FedCablingHistosUsingDb::" << __func__ << "]"
	   << " Invalid FedId/Ch! " 
	   << " Connection not established for"
	   << " Crate/FEC/Ring/CCU/Module/LLDchan: "	
	   << fec_path.fecCrate_ << "/"
	   << fec_path.fecSlot_ << "/"
	   << fec_path.fecRing_ << "/"
	   << fec_path.ccuAddr_ << "/"
	   << fec_path.ccuChan_ << "/"
	   << fec_path.channel_;
      continue;
    }
      
    // Some debug
    cout << endl // LogTrace(mlDqmClient_)
	 << "[FedCablingHistosUsingDb::" << __func__ << "]"
	 << " Built new FedChannelConnectionDescription with "
	 << " FedId/Ch: " 
	 << ianal->second.fedId() << "/"
	 << ianal->second.fedCh()
	 << " and Crate/FEC/Ring/CCU/Module/LLDchan: "	
	 << fec_path.fecCrate_ << "/"
	 << fec_path.fecSlot_ << "/"
	 << fec_path.fecRing_ << "/"
	 << fec_path.ccuAddr_ << "/"
	 << fec_path.ccuChan_ << "/"
	 << fec_path.channel_;

  }

  cout << endl // LogTrace(mlDqmClient_)
       << "[FedCablingHistosUsingDb::" << __func__ << "]"
       << " Added " << mapping().size()
       << " entries to FED-FEC mapping object!";

}

// -----------------------------------------------------------------------------
/** */
void FedCablingHistosUsingDb::update( SiStripConfigDb::FedDescriptions& feds ) {

  // Iterate through feds and disable all channels 
  SiStripConfigDb::FedDescriptions::iterator ifed;
  try {
    for ( ifed = feds.begin(); ifed != feds.end(); ifed++ ) {
      for ( uint16_t ichan = 0; ichan < sistrip::FEDCH_PER_FED; ichan++ ) {
	Fed9U::Fed9UAddress addr( ichan );
	Fed9U::Fed9UAddress addr0( ichan, static_cast<Fed9U::u8>(0) );
	Fed9U::Fed9UAddress addr1( ichan, static_cast<Fed9U::u8>(1) );
	(*ifed)->setFedFeUnitDisable( addr, true );
	(*ifed)->setApvDisable( addr0, true );
	(*ifed)->setApvDisable( addr1, true );
      }
    }
  } catch( ICUtils::ICException& e ) {
    cout << e.what() << endl;
  }
	    
  // Iterate through feds and enable connected channels
  for ( ifed = feds.begin(); ifed != feds.end(); ifed++ ) {
    for ( uint16_t ichan = 0; ichan < sistrip::FEDCH_PER_FED; ichan++ ) {
      
      // Retrieve FEC key from FED-FEC map
      uint32_t fec_key = 0;
      uint32_t fed_key = SiStripFedKey::key( static_cast<uint16_t>((*ifed)->getFedId()), ichan );
      FedToFecMap::const_iterator ifec = mapping().find(fed_key);
      if ( ifec != mapping().end() ) { fec_key = ifec->second; }
      else {
	stringstream ss;
	ss << "[FedCablingHistosUsingDb::" << __func__ << "]"
	   << " Unable to find FecKey for FedKey/Id/Ch: 0x"
	   << hex << setw(8) << setfill('0') << fed_key << dec << "/"
	   << (*ifed)->getFedId() << "/" << ichan;
	cerr << ss.str() << endl; // edm::LogWarning(mlDqmClient_) << ss.str();
	continue; 
      }
      
      // Enable front-end unit and channel
      map<uint32_t,FedCablingAnalysis>::const_iterator iter = data_.find( fec_key );
      if ( iter != data_.end() ) { 

	Fed9U::Fed9UAddress addr( ichan );
	Fed9U::Fed9UAddress addr0( ichan, static_cast<Fed9U::u8>(0) );
	Fed9U::Fed9UAddress addr1( ichan, static_cast<Fed9U::u8>(1) );
	(*ifed)->setFedFeUnitDisable( addr, false );
	(*ifed)->setApvDisable( addr0, false );
	(*ifed)->setApvDisable( addr1, false );

	SiStripFecKey::Path path = SiStripFecKey::path( fec_key );
	cout << endl // LogTrace(mlDqmClient_)
	     << "[FedCablingHistosUsingDb::" << __func__ << "]"
	     << " Eanbled FED channel for FedKey/Id/Ch: 0x"
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

    } // channel loop
  } // fed loop
  
}





