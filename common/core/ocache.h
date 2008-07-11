// Jonathan Eastep (eastep@mit.edu) 
// 04.09.08
//
// This file implements a dynamically adaptive cache. The Organic Cache
// (OCache) consists of an instruction L1 and data L1 which share a pool
// of cache banks. Each bank implements a "way" in terms of associativity,
// so there are n ways split arbitrarily between the icache and dcache. 
// Note: the number of physical sets in the OCache is fixed, so it adjusts
// simultaneously the cache sizes when it reapportions the banks. 
// The OCache monitors cache access statistics to automatically manage
// bank reapportioning. 


#ifndef OCACHE_H
#define OCACHE_H

#include <iostream>
#include <fstream>
#include <sstream>

#include "pin.H"
#include "cache.h"
#include "pin_profile.H"


/* ===================================================================== */
/* Externally defined variables */
/* ===================================================================== */

extern LEVEL_BASE::KNOB<bool> g_knob_icache_ignore_size;
extern LEVEL_BASE::KNOB<bool> g_knob_dcache_ignore_size;
extern LEVEL_BASE::KNOB<bool> g_knob_dcache_track_loads;
extern LEVEL_BASE::KNOB<bool> g_knob_dcache_track_stores;
extern LEVEL_BASE::KNOB<bool> g_knob_icache_track_insts;
extern LEVEL_BASE::KNOB<bool> g_knob_enable_dcache_modeling;
extern LEVEL_BASE::KNOB<bool> g_knob_enable_icache_modeling;

extern LEVEL_BASE::KNOB<UINT32> g_knob_cache_size;
extern LEVEL_BASE::KNOB<UINT32> g_knob_line_size;
extern LEVEL_BASE::KNOB<UINT32> g_knob_associativity;
extern LEVEL_BASE::KNOB<UINT32> g_knob_mutation_interval;
extern LEVEL_BASE::KNOB<UINT32> g_knob_dcache_threshold_hit;
extern LEVEL_BASE::KNOB<UINT32> g_knob_dcache_threshold_miss;
extern LEVEL_BASE::KNOB<UINT32> g_knob_dcache_size;
extern LEVEL_BASE::KNOB<UINT32> g_knob_dcache_associativity;
extern LEVEL_BASE::KNOB<UINT32> g_knob_dcache_max_search_depth;
extern LEVEL_BASE::KNOB<UINT32> g_knob_icache_threshold_hit;
extern LEVEL_BASE::KNOB<UINT32> g_knob_icache_threshold_miss;
extern LEVEL_BASE::KNOB<UINT32> g_knob_icache_size;
extern LEVEL_BASE::KNOB<UINT32> g_knob_icache_associativity;
extern LEVEL_BASE::KNOB<UINT32> g_knob_icache_max_search_depth; 

/* ===================================================================== */
/* Organic Cache Class */
/* ===================================================================== */



// organic cache

class OCache
{
   private:
      typedef Cache<CACHE_SET::RoundRobin<16>, k_MEGA, 64, CACHE_ALLOC::k_STORE_ALLOCATE> 
              RRSACache;

   public:
      typedef enum
      {
         k_COUNTER_MISS = 0,
         k_COUNTER_HIT = 1,
         k_COUNTER_NUM
      } Counter;

      typedef  COUNTER_ARRAY<UINT64, k_COUNTER_NUM> CounterArray;

      // holds the counters with misses and hits
      // conceptually this is an array indexed by instruction address
      COMPRESSOR_COUNTER<ADDRINT, UINT32, CounterArray> dcache_profile;
      COMPRESSOR_COUNTER<ADDRINT, UINT32, CounterArray> icache_profile;

   private:
      RRSACache    *dl1;
      RRSACache    *il1;

      UINT32      cache_size;
      UINT32      line_size;
      UINT32      associativity;
   
      UINT32      mutation_interval;
      UINT64      dcache_total_accesses;
      UINT32      dcacheAccesses;
      UINT64      dcache_total_misses;
      UINT32      dcache_misses;
      UINT64      icache_total_accesses;
      UINT32      icache_accesses;
      UINT64      icache_total_misses;
      UINT32      icache_misses;
      UINT64      total_resize_evictions;

      UINT32      last_dcache_misses;
      UINT32      last_icache_misses;

      string      name;

   private:

      inline VOID resetIntervalCounters()
      {
         dcache_misses = 0;
         dcacheAccesses = 0;
         icache_misses = 0;
         icache_accesses = 0;
      }

      // These functions define the concurrent evolution heuristic of the dcache and icache
      VOID evolveNaive();
      VOID evolveDataIntensive();

      // This function increments an interval counter (triggered by some cache event)
      // When the counter reaches a threshold, it fires an evolution function
      VOID mutationRuntime();

      // These functions access the dcache and icache
      // You can access either a single line or multi lines
      // Fast accesses skip adding the access to the profiler
      bool dCacheLoadMulti(ADDRINT addr, UINT32 size, UINT32 inst_id);
      bool dCacheLoadMultiFast(ADDRINT addr, UINT32 size);
      bool dCacheLoadSingle(ADDRINT addr, UINT32 inst_id);
      bool dCacheLoadSingleFast(ADDRINT addr);
      bool dCacheStoreMulti(ADDRINT addr, UINT32 size, UINT32 inst_id);
      bool dCacheStoreMultiFast(ADDRINT addr, UINT32 size);
      bool dCacheStoreSingle(ADDRINT addr, UINT32 inst_id);
      bool dCacheStoreSingleFast(ADDRINT addr);
      bool iCacheLoadMulti(ADDRINT addr, UINT32 size, UINT32 inst_id);
      bool iCacheLoadMultiFast(ADDRINT addr, UINT32 size);
      bool iCacheLoadSingle(ADDRINT addr, UINT32 inst_id);
      bool iCacheLoadSingleFast(ADDRINT addr);

   public:

      // These are just wrappers around the Cache class equivalents for the OCache dcache field
      UINT32 dCacheSize() { return dl1->getCacheSize(); }
      UINT32 dCacheLineSize() { return dl1->getLineSize(); }
      UINT32 dCacheAssociativity() { return dl1->getNumWays(); }
      UINT32 dCacheGetSetPtr(UINT32 set_index) { return dl1->getSetPtr(set_index); }
      VOID   dCacheSetSetPtr(UINT32 set_index, UINT32 value) { dl1->setSetPtr(set_index, value); }
      string dCacheStatsLong(string prefix, CacheBase::CacheType type) 
      { return dl1->statsLong(prefix,type); }

      // These are just wrappers around the Cache class equivalents for the OCache icache field
      UINT32 iCacheSize() { return il1->getCacheSize(); }
      UINT32 iCacheLineSize() { return il1->getLineSize(); }
      UINT32 iCacheAssociativity() { return il1->getNumWays(); }
      UINT32 iCacheGetSetPtr(UINT32 set_index) { return il1->getSetPtr(set_index); }
      VOID   iCacheSetSetPtr(UINT32 set_index, UINT32 value) { il1->setSetPtr(set_index, value); } 
      string iCacheStatsLong(string prefix, CacheBase::CacheType type) { return il1->statsLong(prefix,type); }

      string statsLong();  

      // Constructor
      OCache(std::string name, UINT32 size, UINT32 line_bytes, UINT32 assoc, UINT32 mutate_interval,
             UINT32 dcache_threshold_hit_value, UINT32 dcache_threshold_miss_value, UINT32 dcache_size, 
             UINT32 dcache_associativity, UINT32 dcache_max_search_depth, UINT32 icache_threshold_hit_value, 
             UINT32 icache_threshold_miss_value, UINT32 icache_size, UINT32 icache_associativity, 
             UINT32 icache_max_search_depth);

      // These functions provide the public interface to accessing the caches
      bool runICacheLoadModel(ADDRINT i_addr, UINT32 size);
      bool runDCacheLoadModel(ADDRINT d_addr, UINT32 size);
      bool runDCacheStoreModel(ADDRINT d_addr, UINT32 size);

      // This function is called at the end of simulation
      void fini(int code, VOID *v, ofstream& out);

};



/* ===================================================================== */
/* Global interface and wrappers: definitions */
/* ===================================================================== */


bool runICacheLoadModel(ADDRINT i_addr, UINT32 size);
bool runDCacheLoadModel(ADDRINT d_addr, UINT32 size);
bool runDCacheStoreModel(ADDRINT d_addr, UINT32 size);

VOID oCacheModelInit();
VOID oCacheModelFini(int code, VOID *v, ofstream& out);

 
#endif