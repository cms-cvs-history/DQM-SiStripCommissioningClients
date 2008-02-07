#ifndef DQM_SiStripCommissioningClients_FedCablingHistograms_H
#define DQM_SiStripCommissioningClients_FedCablingHistograms_H

#include "DQM/SiStripCommissioningClients/interface/CommissioningHistograms.h"
#include "DQM/SiStripCommissioningSummary/interface/FedCablingSummaryFactory.h"
#include "CondFormats/SiStripObjects/interface/FedCablingAnalysis.h"

class MonitorUserInterface;
class DaqMonitorBEInterface;

class FedCablingHistograms : virtual public CommissioningHistograms {

 public:
  
  FedCablingHistograms( MonitorUserInterface* );
  FedCablingHistograms( DaqMonitorBEInterface* );
  virtual ~FedCablingHistograms();
  
  typedef SummaryPlotFactory<FedCablingAnalysis*> Factory;
  typedef std::map<uint32_t,FedCablingAnalysis*> Analyses;

  /** */
  void histoAnalysis( bool debug );

  /** */
  void printAnalyses();
  
  /** */
  void createSummaryHisto( const sistrip::Monitorable&,
			   const sistrip::Presentation&,
			   const std::string& top_level_dir,
			   const sistrip::Granularity& );
  
 protected: 
  
  Analyses data_;
  
  std::auto_ptr<Factory> factory_;

};

#endif // DQM_SiStripCommissioningClients_FedCablingHistograms_H


