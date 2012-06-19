#pragma once

#include <string>
using std::string;

// Forward declaration
namespace HybridProtocol_PPMOSI_SS
{
   class L2CacheCntlr;
   class MemoryManager;
}

#include "tile.h"
#include "cache.h"
#include "cache_line_info.h"
#include "shmem_msg.h"
#include "mem_component.h"
#include "semaphore.h"
#include "lock.h"
#include "fixed_types.h"
#include "shmem_perf_model.h"
#include "cache_replacement_policy.h"
#include "cache_hash_fn.h"

namespace HybridProtocol_PPMOSI_SS
{

class L1CacheCntlr
{
public:
   L1CacheCntlr(MemoryManager* memory_manager,
                UInt32 cache_line_size,
                UInt32 l1_icache_size,
                UInt32 l1_icache_associativity,
                string l1_icache_replacement_policy,
                UInt32 l1_icache_access_delay,
                bool l1_icache_track_miss_types,
                UInt32 l1_dcache_size,
                UInt32 l1_dcache_associativity,
                string l1_dcache_replacement_policy,
                UInt32 l1_dcache_access_delay,
                bool l1_dcache_track_miss_types,
                float frequency);
   ~L1CacheCntlr();

   Cache* getL1ICache() { return _l1_icache; }
   Cache* getL1DCache() { return _l1_dcache; }

   void setL2CacheCntlr(L2CacheCntlr* l2_cache_cntlr);

   bool processMemOpFromCore(MemComponent::Type mem_component,
                             Core::lock_signal_t lock_signal,
                             Core::mem_op_t mem_op_type, 
                             IntPtr ca_address, UInt32 offset,
                             Byte* data_buf, UInt32 data_length,
                             bool modeled);

   CacheState::Type getCacheLineState(MemComponent::Type mem_component, IntPtr address);
   void setCacheLineState(MemComponent::Type mem_component, IntPtr address, CacheState::Type cstate);
   void insertCacheLine(MemComponent::Type mem_component,
                        IntPtr address, CacheState::Type cstate, Byte* fill_buf,
                        bool* eviction, IntPtr* evicted_address);
   void invalidateCacheLine(MemComponent::Type mem_component, IntPtr address);
   void lockCacheLine(MemComponent::Type mem_component, IntPtr address);
   void unlockCacheLine(MemComponent::Type mem_component, IntPtr address);

   void acquireLock(MemComponent::Type mem_component);
   void releaseLock(MemComponent::Type mem_component);

private:
   MemoryManager* _memory_manager;
   Cache* _l1_icache;
   Cache* _l1_dcache;
   CacheReplacementPolicy* _l1_icache_replacement_policy_obj;
   CacheReplacementPolicy* _l1_dcache_replacement_policy_obj;
   CacheHashFn* _l1_icache_hash_fn_obj;
   CacheHashFn* _l1_dcache_hash_fn_obj;
   L2CacheCntlr* _l2_cache_cntlr;

   Lock _l1_icache_lock;
   Lock _l1_dcache_lock;

   void accessCache(MemComponent::Type mem_component,
                    Core::lock_signal_t lock_signal, Core::mem_op_t mem_op_type, 
                    IntPtr ca_address, UInt32 offset,
                    Byte* data_buf, UInt32 data_length);
   pair<bool,Cache::MissType> getMemOpStatusInL1Cache(MemComponent::Type mem_component, IntPtr address,
                                                      Core::mem_op_t mem_op_type);

   Cache* getL1Cache(MemComponent::Type mem_component);
   void invalidateCacheLine(Cache* l1_cache, IntPtr address);
   void lockCacheLine(Cache* l1_cache, IntPtr address);
   void unlockCacheLine(Cache* l1_cache, IntPtr address);

   // Utilities
   tile_id_t getTileId();
   UInt32 getCacheLineSize();
   MemoryManager* getMemoryManager()   { return _memory_manager; }
   ShmemPerfModel* getShmemPerfModel();
};

}
