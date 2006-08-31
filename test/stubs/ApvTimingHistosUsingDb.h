#ifndef DQM_SiStripCommissioningClients_ApvTimingHistosUsingDb_H
#define DQM_SiStripCommissioningClients_ApvTimingHistosUsingDb_H

#include "DQM/SiStripCommissioningClients/interface/ApvTimingHistograms.h"
#include "DQM/SiStripCommissioningClients/test/stubs/CommissioningHistosUsingDb.h"
#include "OnlineDB/SiStripConfigDb/interface/SiStripConfigDb.h"
#include <boost/cstdint.hpp>
#include <string>
#include <map>

class ApvTimingHistosUsingDb : public ApvTimingHistograms,
			       public CommissioningHistosUsingDb 
{
  
 public:
  
  ApvTimingHistosUsingDb( MonitorUserInterface*,
			  std::string confdb,
			  std::string partition,
			  uint32_t major,
			  uint32_t minor );
  virtual ~ApvTimingHistosUsingDb();

  virtual void uploadToConfigDb();
  
 private:

  void pllDelays( std::map<uint32_t,uint32_t>& );
  
  void update( const std::map<uint32_t,uint32_t>&,
	       SiStripConfigDb::DeviceDescriptions& );
  
  
};

#endif // DQM_SiStripCommissioningClients_ApvTimingHistosUsingDb_H

