#ifndef DQM_SiStripCommissioningClients_FedCablingHistosUsingDb_H
#define DQM_SiStripCommissioningClients_FedCablingHistosUsingDb_H

#include "DQM/SiStripCommissioningClients/interface/FedCablingHistograms.h"
#include "DQM/SiStripCommissioningClients/test/stubs/CommissioningHistosUsingDb.h"
#include "OnlineDB/SiStripConfigDb/interface/SiStripConfigDb.h"
#include <boost/cstdint.hpp>
#include <string>
#include <map>

class FedCablingHistosUsingDb : public FedCablingHistograms,
				public CommissioningHistosUsingDb 
{
  
 public:
  
  FedCablingHistosUsingDb( MonitorUserInterface*,
			   std::string confdb,
			   std::string partition,
			   uint32_t major,
			   uint32_t minor );
  virtual ~FedCablingHistosUsingDb();
  
  virtual void uploadToConfigDb();
  
 private:
  
  void update( SiStripConfigDb::FedConnections& );
  
  
};

#endif // DQM_SiStripCommissioningClients_FedCablingHistosUsingDb_H

