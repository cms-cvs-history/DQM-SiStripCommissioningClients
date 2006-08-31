#ifndef DQM_SiStripCommissioningClients_OptoScanHistosUsingDb_H
#define DQM_SiStripCommissioningClients_OptoScanHistosUsingDb_H

#include "DQM/SiStripCommissioningClients/interface/OptoScanHistograms.h"
#include "DQM/SiStripCommissioningClients/test/stubs/CommissioningHistosUsingDb.h"
#include "OnlineDB/SiStripConfigDb/interface/SiStripConfigDb.h"
#include <boost/cstdint.hpp>
#include <string>
#include <map>

class OptoScanHistosUsingDb : public OptoScanHistograms,
			      public CommissioningHistosUsingDb 
{
  
 public:
  
  OptoScanHistosUsingDb( MonitorUserInterface*,
			 std::string confdb,
			 std::string partition,
			 uint32_t major,
			 uint32_t minor );
  virtual ~OptoScanHistosUsingDb();

  virtual void uploadToConfigDb();
  
 private:
  
  void update( SiStripConfigDb::DeviceDescriptions& );
  
  
};

#endif // DQM_SiStripCommissioningClients_OptoScanHistosUsingDb_H

