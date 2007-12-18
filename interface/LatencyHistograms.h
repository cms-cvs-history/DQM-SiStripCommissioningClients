#ifndef DQM_SiStripCommissioningClients_LatencyHistograms_H
#define DQM_SiStripCommissioningClients_LatencyHistograms_H

#include "DQM/SiStripCommissioningClients/interface/CommissioningHistograms.h"
#include "DQM/SiStripCommissioningSummary/interface/LatencySummaryFactory.h"
#include "DQM/SiStripCommissioningAnalysis/interface/LatencyAnalysis.h"

class MonitorUserInterface;

class LatencyHistograms : public CommissioningHistograms {

 public:
  
  LatencyHistograms( DaqMonitorBEInterface* );
  LatencyHistograms( MonitorUserInterface* );
  virtual ~LatencyHistograms();
  
  typedef SummaryHistogramFactory<LatencyAnalysis> Factory;
  
  /** */
  void histoAnalysis( bool debug );

  /** */
  void createSummaryHisto( const sistrip::Monitorable&,
                           const sistrip::Presentation&,
                           const std::string& top_level_dir,
                           const sistrip::Granularity& );
  
 protected: 
  
  std::map<uint32_t,LatencyAnalysis> data_;
  
  std::auto_ptr<Factory> factory_;
  
};

#endif // DQM_SiStripCommissioningClients_LatencyHistograms_H
