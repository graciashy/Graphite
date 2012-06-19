#pragma once

#include <string>
using std::string;

#include "mode.h"
#include "fixed_types.h"
#include "shmem_msg.h"
#include "directory_entry.h"

namespace HybridProtocol_PPMOSI_SS
{

class Classifier
{
public:
   enum Type
   {
      PRIVATE_FIXED,
      REMOTE_FIXED,
      RANDOM,
      SHARED_REMOTE,
      SHARED_READ_WRITE_REMOTE,
      LOCALITY_BASED,
      PREDICTIVE_LOCALITY_BASED,
      NUM_TYPES
   };

   Classifier();
   virtual ~Classifier();

   static Type getType()            { return _type; }
   static void setType(string type) { _type = parse(type); }

   static Classifier* create(SInt32 max_hw_sharers);

   virtual Mode getMode(tile_id_t sharer, ShmemMsg::Type req_type, DirectoryEntry* directory_entry) = 0;
   virtual void updateMode(tile_id_t sender, ShmemMsg* shmem_msg, DirectoryEntry* directory_entry) = 0;

private:
   static Type _type;
   static Type parse(string type);
};

}
