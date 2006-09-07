#ifndef DQM_SiStripCommissioningClients_SiStripCommissioningDbClient_H
#define DQM_SiStripCommissioningClients_SiStripCommissioningDbClient_H

#include "DQM/SiStripCommissioningClients/interface/SiStripCommissioningClient.h"
#include <boost/cstdint.hpp>
#include<string>

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
  xdata::String confdb_;
  xdata::String partition_;
  xdata::UnsignedLong major_;
  xdata::UnsignedLong minor_;

};

#endif // DQM_SiStripCommissioningClients_SiStripCommissioningDbClient_H

