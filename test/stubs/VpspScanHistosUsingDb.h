#ifndef DQM_SiStripCommissioningClients_VpspScanHistosUsingDb_H
#define DQM_SiStripCommissioningClients_VpspScanHistosUsingDb_H

#include "DQM/SiStripCommissioningClients/interface/VpspScanHistograms.h"
#include "DQM/SiStripCommissioningClients/test/stubs/CommissioningHistosUsingDb.h"
#include "OnlineDB/SiStripConfigDb/interface/SiStripConfigDb.h"
#include <boost/cstdint.hpp>
#include <map>

class VpspScanHistosUsingDb : public VpspScanHistograms,
			      public CommissioningHistosUsingDb 
{
  
 public:
  
  VpspScanHistosUsingDb( MonitorUserInterface*,
			 std::string confdb,
			 std::string partition,
			 uint32_t major,
			 uint32_t minor );
  virtual ~VpspScanHistosUsingDb();

  virtual void uploadToConfigDb();
  
 private:

  void update( SiStripConfigDb::DeviceDescriptions& );
  
  
};

#endif // DQM_SiStripCommissioningClients_VpspScanHistosUsingDb_H

