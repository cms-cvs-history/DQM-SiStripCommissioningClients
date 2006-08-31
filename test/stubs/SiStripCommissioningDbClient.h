#ifndef DQM_SiStripCommissioningClients_SiStripCommissioningDbClient_H
#define DQM_SiStripCommissioningClients_SiStripCommissioningDbClient_H

#include "DQM/SiStripCommissioningClients/interface/SiStripCommissioningClient.h"

class SiStripConfigDb;

class SiStripCommissioningDbClient : public SiStripCommissioningClient {
  
 public:
  
  XDAQ_INSTANTIATOR();
  
  SiStripCommissioningDbClient( xdaq::ApplicationStub* );
  virtual ~SiStripCommissioningDbClient();

  /** */
  virtual void uploadToConfigDb();

 protected:
  
  /** */
  virtual void createHistograms( const sistrip::Task& task ) const;
  
  // Extract db connections params
  std::string confdb_;
  std::string partition_;
  uint32_t major_;
  uint32_t minor_;
  
};

#endif // DQM_SiStripCommissioningClients_SiStripCommissioningDbClient_H

