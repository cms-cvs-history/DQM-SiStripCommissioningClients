#ifndef DQM_SiStripCommissioningClients_CommissioningHistosUsingDb_H
#define DQM_SiStripCommissioningClients_CommissioningHistosUsingDb_H

#include <boost/cstdint.hpp>
#include <string>

class SiStripConfigDb;

class CommissioningHistosUsingDb {
  
 public:
  
  CommissioningHistosUsingDb( std::string confdb,
			      std::string partition,
			      uint32_t major,
			      uint32_t minor );
  virtual ~CommissioningHistosUsingDb();
  
 protected:

  /** */
  SiStripConfigDb* db_;

 private: 

  CommissioningHistosUsingDb() {;}

};

#endif // DQM_SiStripCommissioningClients_CommissioningHistosUsingDb_H
