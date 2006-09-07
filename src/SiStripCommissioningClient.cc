#include "DQM/SiStripCommissioningClients/interface/SiStripCommissioningClient.h"
#include "DQM/SiStripCommissioningClients/interface/SiStripCommissioningWebClient.h"
#include "DQM/SiStripCommissioningClients/interface/CommissioningHistograms.h"
#include "DQM/SiStripCommissioningClients/interface/ApvTimingHistograms.h"
#include "DQM/SiStripCommissioningClients/interface/FedCablingHistograms.h"
#include "DQM/SiStripCommissioningClients/interface/FedTimingHistograms.h"
#include "DQM/SiStripCommissioningClients/interface/OptoScanHistograms.h"
#include "DQM/SiStripCommissioningClients/interface/PedestalsHistograms.h"
#include "DQM/SiStripCommissioningClients/interface/VpspScanHistograms.h"
#include "DQMServices/Core/interface/MonitorUserInterface.h"
#include <SealBase/Callback.h>
// #include "FWCore/Utilities/interface/Presence.h"
// #include "FWCore/Utilities/interface/PresenceFactory.h"
// #include "FWCore/Utilities/interface/ProblemTracker.h"
// #include <boost/shared_ptr.hpp>

// This line is necessary
XDAQ_INSTANTIATOR_IMPL(SiStripCommissioningClient);

using namespace std;

// -----------------------------------------------------------------------------
/** */
SiStripCommissioningClient::SiStripCommissioningClient( xdaq::ApplicationStub* stub ) 
  : DQMBaseClient( stub, "SiStripCommissioningClient", "localhost", 9090 ),
    web_(0),
    histos_(0),
    task_(sistrip::UNKNOWN_TASK)
{
  xgi::bind( this, &SiStripCommissioningClient::handleWebRequest, "Request" );
  
  //   // Service allows use of MessageLogger
  //   edm::AssertHandler ah;
  //   boost::shared_ptr<edm::Presence> message = boost::shared_ptr<edm::Presence>( edm::PresenceFactory::get()->makePresence("MessageServicePresence").release() );
  
}

// -----------------------------------------------------------------------------
/** */
SiStripCommissioningClient::~SiStripCommissioningClient() {
  if ( web_ ) { delete web_; }
  if ( histos_ ) { delete histos_; }
}

// -----------------------------------------------------------------------------
/** Called whenever the client enters the "Configured" state. */
void SiStripCommissioningClient::configure() {
  cout << "[" << __PRETTY_FUNCTION__ << "]" << endl;
  web_ = new SiStripCommissioningWebClient( this,
					    getContextURL(),
					    getApplicationURL(), 
					    &mui_ );
}

// -----------------------------------------------------------------------------
/** Called whenever the client enters the "Enabled" state. */
void SiStripCommissioningClient::newRun() {
  cout << "[" << __PRETTY_FUNCTION__ << "]" << endl;
  ( this->upd_ )->registerObserver( this ); 
}

// -----------------------------------------------------------------------------
/** Called whenever the client enters the "Halted" state. */
void SiStripCommissioningClient::endRun() {
  cout << "[" << __PRETTY_FUNCTION__ << "]" << endl;
  unsubscribeAll(); 
  if ( histos_ ) { delete histos_; histos_ = 0; }
}

// -----------------------------------------------------------------------------
/** Called by the "Updater" following each update. */
void SiStripCommissioningClient::onUpdate() const {
  cout << "[" << __PRETTY_FUNCTION__ << "]"
       << " Number of updates: " << mui_->getNumUpdates() << endl;
  
  // Retrieve a list of all subscribed histograms
  if ( mui_ ) { mui_->subscribe( "*" ); }
  vector<string> contents;
  mui_->getContents( contents ); 
  
  if ( contents.empty() ) { return; }

  // Extract commissioning task from added contents
  if ( task_ == sistrip::UNKNOWN_TASK ) { task_ = extractTask( contents ); }

  // Create histograms for given commissioning task
  createHistograms( task_ );
  
  // Create collation histograms based on added contents
  if ( histos_ ) { histos_->createCollations( contents ); }
  
}

// -----------------------------------------------------------------------------
/** Extract "commissioning task" string from "added contents". */
sistrip::Task SiStripCommissioningClient::extractTask( const vector<string>& contents ) const {
  cout << "[" << __PRETTY_FUNCTION__ << "]" << endl;
  
  // Iterate through added contents
  vector<string>::const_iterator istr = contents.begin();
  while ( istr != contents.end() ) {
    
    // Search for "commissioning task" string
    string::size_type pos = istr->find( sistrip::commissioningTask_ );
    cout << "[" << __PRETTY_FUNCTION__ << "]"
	 << " Looking for 'SiStripCommissioningTask' within string: " 
	 << *istr << endl;
    if ( pos != string::npos ) { 
      // Extract commissioning task from string 
      string value = istr->substr( pos+sistrip::commissioningTask_.size()+1, string::npos ); 
      if ( !value.empty() ) { 
	cout << "[" << __PRETTY_FUNCTION__ << "]"
	     << " Found string " <<  istr->substr(pos,string::npos)
	     << " with value " << value << endl;
	if ( !(mui_->get(sistrip::root_+"/"+istr->substr(pos,string::npos))) ) { 
	  mui_->setCurrentFolder(sistrip::root_);
	  mui_->getBEInterface()->bookString( istr->substr(pos,string::npos), value ); 
	}
	return SiStripHistoNamingScheme::task( value ); 
      }
    }
    istr++;
    
  }
  return sistrip::UNKNOWN_TASK;
}

// -----------------------------------------------------------------------------
/** Create histograms for given commissioning task. */
void SiStripCommissioningClient::createHistograms( const sistrip::Task& task ) const {

  // Check if object already exists
  if ( histos_ ) { return; }
  
  // Create corresponding "commissioning histograms" object 
  if      ( task == sistrip::APV_TIMING )     { histos_ = new ApvTimingHistograms( mui_ ); }
  else if ( task == sistrip::FED_CABLING )    { histos_ = new FedCablingHistograms( mui_ ); }
  else if ( task == sistrip::FED_TIMING )     { histos_ = new FedTimingHistograms( mui_ ); }
  else if ( task == sistrip::OPTO_SCAN )      { histos_ = new OptoScanHistograms( mui_ ); }
  else if ( task == sistrip::PEDESTALS )      { histos_ = new PedestalsHistograms( mui_ ); }
  else if ( task == sistrip::VPSP_SCAN )      { histos_ = new VpspScanHistograms( mui_ ); }
  else if ( task == sistrip::UNDEFINED_TASK ) { histos_ = 0; }
  else if ( task == sistrip::UNKNOWN_TASK ) {
    histos_ = 0;
    cerr << "[" << __PRETTY_FUNCTION__ << "]"
	 << " Unknown commissioning task!" << endl;
  }
  
}

// -----------------------------------------------------------------------------
/** General access to client info. */
void SiStripCommissioningClient::general( xgi::Input* in, xgi::Output* out ) throw ( xgi::exception::Exception ) {
  if ( web_ ) { web_->Default( in, out ); }
  else { cerr << "[" << __PRETTY_FUNCTION__ << "]"
	      << " NULL pointer to WebPage!" << endl; }
}

// -----------------------------------------------------------------------------
/** */
void SiStripCommissioningClient::handleWebRequest( xgi::Input* in, xgi::Output* out ) {
  if ( web_ ) { web_->handleRequest(in, out); }
  else { cerr << "[" << __PRETTY_FUNCTION__ << "]"
	      << " NULL pointer to WebPage!" << endl; }
}

// -----------------------------------------------------------------------------
/** */
void SiStripCommissioningClient::histoAnalysis() {

  if ( !histos_ ) { 
    cerr << "[" << __PRETTY_FUNCTION__ << "]" 
	 << " NULL pointer to CommissioningHistograms!" << endl; 
    return;
  }
  
  seal::Callback action; 
  action = seal::CreateCallback( histos_, 
				 &CommissioningHistograms::histoAnalysis 
				 ); // no arguments
  
  if ( mui_ ) { 
    mui_->addCallback(action); 
    cout << "[" << __PRETTY_FUNCTION__ << "]" 
	 << " Scheduling this action..." << endl;
  } else { 
    cerr << "[" << __PRETTY_FUNCTION__ << "]" 
	 << " NULL pointer to MonitorUserInterface!" << endl; 
    return;
  }
  
}

// -----------------------------------------------------------------------------
/** */
void SiStripCommissioningClient::subscribeAll( string pattern ) {

  if ( pattern == "" ) { pattern = "*/" + sistrip::root_ + "/*"; }

  seal::Callback action;
  action = seal::CreateCallback( this, 
				 &SiStripCommissioningClient::subscribe,
				 pattern ); //@@ argument list

//   action = seal::CreateCallback( histos_, 
// 				 &CommissioningHistograms::subscribe,
// 				 mui_, pattern ); //@@ argument list
  
  if ( mui_ ) { 
    mui_->addCallback(action); 
    cout << "[" << __PRETTY_FUNCTION__ << "]" 
	 << " Scheduling this action..." << endl;
  } else { 
    cerr << "[" << __PRETTY_FUNCTION__ << "]" 
	 << " NULL pointer to MonitorUserInterface!" << endl; 
    return;
  }
  
}

// -----------------------------------------------------------------------------
/** */
void SiStripCommissioningClient::unsubscribeAll( string pattern ) {

  if ( pattern == "" ) { pattern = "*/" + sistrip::root_ + "/*"; }

  seal::Callback action;
  action = seal::CreateCallback( this, 
				 &SiStripCommissioningClient::unsubscribe,
				 pattern ); //@@ argument list
  
  if ( mui_ ) { 
    mui_->addCallback(action); 
    cout << "[" << __PRETTY_FUNCTION__ << "]" 
	 << " Scheduling this action..." << endl;
  } else { 
    cerr << "[" << __PRETTY_FUNCTION__ << "]" 
	 << " NULL pointer to MonitorUserInterface!" << endl; 
    return;
  }
  
}

// -----------------------------------------------------------------------------
/** */
void SiStripCommissioningClient::saveHistos( string name ) {

  seal::Callback action;
  action = seal::CreateCallback( this, 
				 &SiStripCommissioningClient::save,
				 name ); //@@ argument list

  if ( mui_ ) { 
    mui_->addCallback(action); 
    cout << "[" << __PRETTY_FUNCTION__ << "]" 
	 << " Scheduling this action..." << endl;
  } else { 
    cerr << "[" << __PRETTY_FUNCTION__ << "]" 
	 << " NULL pointer to MonitorUserInterface!" << endl; 
    return;
  }
  
}

// -----------------------------------------------------------------------------
/** */
void SiStripCommissioningClient::createSummaryHisto( sistrip::SummaryHisto histo, 
						     sistrip::SummaryType type, 
						     string top_level_dir,
						     sistrip::Granularity gran ) {
  
  if ( !histos_ ) { 
    cerr << "[" << __PRETTY_FUNCTION__ << "]" 
	 << " NULL pointer to CommissioningHistograms!" << endl; 
    return;
  }
  
  pair<sistrip::SummaryHisto,sistrip::SummaryType> summ0(histo,type);
  pair<string,sistrip::Granularity> summ1(top_level_dir,gran);
  seal::Callback action;
  action = seal::CreateCallback( histos_, 
				 &CommissioningHistograms::createSummaryHisto,
				 summ0, summ1 ); //@@ argument list
  
  if ( mui_ ) { 
    mui_->addCallback(action); 
    cout << "[" << __PRETTY_FUNCTION__ << "]" 
	 << " Scheduling this action..." << endl;
  } else { 
    cerr << "[" << __PRETTY_FUNCTION__ << "]" 
	 << " NULL pointer to MonitorUserInterface!" << endl; 
    return;
  }
  
}

// -----------------------------------------------------------------------------
/** */
void SiStripCommissioningClient::uploadToConfigDb() {
  cout << "[" << __PRETTY_FUNCTION__ << "]" 
       << " Dervied implementation to come..." << endl; 
}

// -----------------------------------------------------------------------------
/** */
void SiStripCommissioningClient::subscribe( string pattern ) {
  cout << "[" << __PRETTY_FUNCTION__ << "]" << endl;
  if ( mui_ ) { mui_->subscribe(pattern); }
}

// -----------------------------------------------------------------------------
/** */
void SiStripCommissioningClient::unsubscribe( string pattern ) {
  cout << "[" << __PRETTY_FUNCTION__ << "]" << endl;
  if ( mui_ ) { mui_->unsubscribe(pattern); }
}

// -----------------------------------------------------------------------------
/** */
void SiStripCommissioningClient::save( string name ) {
  stringstream ss; 
  if ( name == "" ) { ss << "Client.root"; }
  else { ss << name; }
  cout << "[" << __PRETTY_FUNCTION__ << "]" 
       << " Saving histogams to file '" << ss.str() << "'..." << endl;
  if ( mui_ ) { mui_->save( ss.str() ); }
}
