#include "DQM/SiStripCommissioningClients/interface/VpspScanHistograms.h"
#include "CondFormats/SiStripObjects/interface/VpspScanAnalysis.h"
#include "DQM/SiStripCommissioningSummary/interface/VpspScanSummaryFactory.h"
#include "DataFormats/SiStripCommon/interface/SiStripConstants.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace sistrip;

// -----------------------------------------------------------------------------
/** */
VpspScanHistograms::VpspScanHistograms( MonitorUserInterface* mui ) 
  : CommissioningHistograms( mui, sistrip::VPSP_SCAN )
{
  factory_ = auto_ptr<VpspScanSummaryFactory>( new VpspScanSummaryFactory );
  LogTrace(mlDqmClient_) 
    << "[VpspScanHistograms::" << __func__ << "]"
    << " Constructing object...";
}

// -----------------------------------------------------------------------------
/** */
VpspScanHistograms::VpspScanHistograms( DaqMonitorBEInterface* bei ) 
  : CommissioningHistograms( bei, sistrip::VPSP_SCAN )
{
  LogTrace(mlDqmClient_) 
    << "[VpspScanHistograms::" << __func__ << "]"
    << " Constructing object...";
}

// -----------------------------------------------------------------------------
/** */
VpspScanHistograms::~VpspScanHistograms() {
  LogTrace(mlDqmClient_) 
    << "[VpspScanHistograms::" << __func__ << "]"
    << " Denstructing object...";
}

// -----------------------------------------------------------------------------	 
/** */	 
void VpspScanHistograms::histoAnalysis( bool debug ) {
  LogTrace(mlDqmClient_)
    << "[VpspScanHistograms::" << __func__ << "]";
  
  // Some initialisation
  uint16_t valid = 0;
  HistosMap::const_iterator iter;
  Analyses::iterator ianal;
  std::map<std::string,uint16_t> errors;
  
  // Clear map holding analysis objects
  for ( ianal = data().begin(); ianal != data().end(); ianal++ ) { 
    if ( ianal->second ) { delete ianal->second; }
  } 
  data().clear();
  
  // Iterate through map containing histograms
  for ( iter = histos().begin(); 
	iter != histos().end(); iter++ ) {
    
    // Check vector of histos is not empty
    if ( iter->second.empty() ) {
      edm::LogWarning(mlDqmClient_) 
	<< "[VpspScanHistograms::" << __func__ << "]"
	<< " Zero histograms found!" << endl;
      continue;
    }
    
    // Retrieve pointers to profile histos for this FED channel 
    std::vector<TH1*> profs;
    Histos::const_iterator ihis = iter->second.begin(); 
    for ( ; ihis != iter->second.end(); ihis++ ) {
      TProfile* prof = ExtractTObject<TProfile>().extract( (*ihis)->me_ );
      if ( prof ) { profs.push_back(prof); }
    } 

    // Perform histo analysis
    VpspScanAnalysis* anal = new VpspScanAnalysis( iter->first );
    anal->analysis( profs );
    data()[iter->first] = anal; 
    if ( anal->isValid() ) { valid++; }
    if ( !anal->getErrorCodes().empty() ) { 
      errors[anal->getErrorCodes()[0]]++;
    }
    
  }
  
  if ( !histos().empty() ) {
    edm::LogVerbatim(mlDqmClient_) 
      << "[VpspScanHistograms::" << __func__ << "]"
      << " Analyzed histograms for " << histos().size() 
      << " FED channels, of which " << valid 
      << " (" << 100 * valid / histos().size()
      << "%) are valid.";
    if ( !errors.empty() ) {
      uint16_t count = 0;
      std::stringstream ss;
      ss << std::endl;
      std::map<std::string,uint16_t>::const_iterator ii;
      for ( ii = errors.begin(); ii != errors.end(); ++ii ) { 
	ss << " " << ii->first << ": " << ii->second << std::endl;
	count += ii->second;
      }
      edm::LogWarning(mlDqmClient_) 
	<< "[VpspScanHistograms::" << __func__ << "]"
	<< " Found " << count << " errors ("
	<< 100 * count / histos().size() << "%): " 
	<< ss.str();
    }
  } else {
    edm::LogWarning(mlDqmClient_) 
      << "[VpspScanHistograms::" << __func__ << "]"
      << " No histograms to analyze!";
  }
  
}

// -----------------------------------------------------------------------------
/** */
void VpspScanHistograms::printAnalyses() {
  Analyses::iterator ianal = data().begin();
  Analyses::iterator janal = data().end();
  for ( ; ianal != janal; ++ianal ) { 
    if ( ianal->second ) { 
      std::stringstream ss;
      ianal->second->print( ss, 1 ); 
      ianal->second->print( ss, 2 ); 
      if ( ianal->second->isValid() ) { LogTrace(mlDqmClient_) << ss.str(); 
      } else { edm::LogWarning(mlDqmClient_) << ss.str(); }
    }
  }
}

// -----------------------------------------------------------------------------
/** */
void VpspScanHistograms::createSummaryHisto( const sistrip::Monitorable& mon, 
					     const sistrip::Presentation& pres, 
					     const std::string& dir,
					     const sistrip::Granularity& gran ) {
  LogTrace(mlDqmClient_)
    << "[VpspScanHistograms::" << __func__ << "]";
  
  // Check view 
  sistrip::View view = SiStripEnumsAndStrings::view(dir);
  if ( view == sistrip::UNKNOWN_VIEW ) { return; }

  // Analyze histograms if not done already
  if ( data().empty() ) { histoAnalysis( false ); }

  // Check
  if ( data().empty() ) { 
    edm::LogError(mlDqmClient_)
      << "[VpspScanHistograms::" << __func__ << "]"
      << " No analyses generated!";
    return;
  }
  
  // Extract data to be histogrammed
  uint32_t xbins = factory()->init( mon, pres, view, dir, gran, data() );
  
  // Use base method to create summary histogram
  TH1* summary = 0;
  if ( pres != sistrip::HISTO_1D ) { summary = histogram( mon, pres, view, dir, xbins ); }
  else { summary = histogram( mon, pres, view, dir, sistrip::FED_ADC_RANGE, 0., sistrip::FED_ADC_RANGE*1. ); }
  
  // Fill histogram with data
  factory()->fill( *summary );
  
}
