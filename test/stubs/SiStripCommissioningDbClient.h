#ifndef DQM_SiStripCommissioningClients_SiStripCommissioningDbClient_H
#define DQM_SiStripCommissioningClients_SiStripCommissioningDbClient_H

#include "DQM/SiStripCommissioningClients/interface/SiStripCommissioningClient.h"
#include <boost/cstdint.hpp>
#include "xdata/include/xdata/UnsignedLong.h"
#include "xdata/include/xdata/Boolean.h"
#include "xdata/include/xdata/String.h"
#include <string>

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
  xdata::Boolean usingDb_;
  xdata::String confdb_;
  xdata::String partition_;
  xdata::UnsignedLong major_;
  xdata::UnsignedLong minor_;

};

#endif // DQM_SiStripCommissioningClients_SiStripCommissioningDbClient_H

