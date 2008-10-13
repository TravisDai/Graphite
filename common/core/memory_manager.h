#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <string>
#include <sstream>
#include <iostream>
#include <queue>
#include "assert.h"
#include <math.h>
#include "debug.h"

// some forward declarations for cross includes
class Core;

// TODO: this is a hack that is due to the fact that network.h 
// is already included by the time this is handled, so NetPacket is 
// never getting defined. Fine some more elegant way to solve this.
typedef struct NetPacket NetPacket;

#include "core.h"
#include "ocache.h"
#include "dram_directory.h"
#include "dram_directory_entry.h"
#include "network.h"
#include "address_home_lookup.h"
#include "cache_state.h"


// define indecies/offsets for shared memory requests. indicies assume integer (4 byte) data sizes

// ***** defines for a shared memory update message (there exist two types, expected updates and unexpected updates)

// size of the request payload (in integer data sizes)
#define SH_MEM_REQ_NUM_INTS_SIZE     (3)

// request type (read or write)
#define SH_MEM_REQ_IDX_REQ_TYPE      (0)

// requested base memory address
#define SH_MEM_REQ_IDX_ADDR          (1)

// number of bytes requested (starting at the base memory address)
#define SH_MEM_REQ_IDX_NUM_BYTES_REQ (2)


// ***** defines for a shared memory update message (there exist two types, expected updates and unexpected updates)

// size of the request payload (in integer data sizes)
#define SH_MEM_UPDATE_NUM_INTS_SIZE     (2)

// new cstate to set cache to
#define SH_MEM_UPDATE_IDX_NEW_CSTATE    (0)

// requested base memory address
#define SH_MEM_UPDATE_IDX_ADDRESS  (1)


// ***** defines for a shared memory update message (there exist two types, expected updates and unexpected updates)

// size of the request payload (in integer data sizes)
#define SH_MEM_ACK_NUM_INTS_SIZE     (2)

// new cstate cache was set to
#define SH_MEM_ACK_IDX_NEW_CSTATE    (0)

// updated base memory address
#define SH_MEM_ACK_IDX_ADDRESS  (1)



extern LEVEL_BASE::KNOB<BOOL> g_knob_simarch_has_shared_mem;
extern LEVEL_BASE::KNOB<UINT32> g_knob_ahl_param;
extern LEVEL_BASE::KNOB<UINT32> g_knob_dram_access_cost;


// TODO: move this into MemoryManager class?
enum shmem_req_t {
  READ, 
  WRITE,
  INVALIDATE,
  NUM_STATES
};

//TODO: refactor into a Payload Class/Struct
//

/*
 * memory coherency message payloads can vary depending
 * on the type of message that needs to be seen
 */

enum sm_payload_t {
	REQUEST,
	UPDATE,
	ACK,
	PAYLOAD_NUM_STATES
};

/* one could make the argument to have one payload
 * with fields that may not be used....
 */
struct RequestPayload {
	shmem_req_t request_type;
	ADDRINT request_address;
	UINT32 request_num_bytes; 
};	

struct UpdatePayload {
	CacheState::cstate_t update_new_cstate;
	ADDRINT update_address;
};

struct AckPayload {
	CacheState::cstate_t ack_new_cstate;
	ADDRINT ack_address;
	//if sent a downgrade message (E->S), but cache
	//no longer has the line, send a bit to tell dram directory
	//to remove it from the sharers' list
	BOOL remove_from_sharers;
};

class MemoryManager
{
 private:
	Core *the_core;
   OCache *ocache;
   DramDirectory *dram_dir;
   AddressHomeLookup *addr_home_lookup;

	//This is here to serialize the requests
	// do not process a new request until finished with current request
	// do not exit MMU until no more incoming requests
	
   UINT64 volatile debug_counter; //a primitive clock for debugging
   bool volatile processing_request_flag;
   int volatile incoming_requests_count;

    /* ============================================= */
    /* Added by George */
    UINT64 dramAccessCost;
	/* ============================================= */

	//FIFO queue
	queue<NetPacket> request_queue;
	void addRequestToQueue( NetPacket packet );
	NetPacket getNextRequest();

	void debugPrintReqPayload(RequestPayload payload);
 public:
//	NetPacket makePacket( PacketType pt, int sender, int receiver, UINT32 payload_size);
//	NetMatch makeNetMatch( PacketType pt, int sender);
	MemoryManager(Core *the_core_arg, OCache *ocache_arg);
	virtual ~MemoryManager();
	bool initiateSharedMemReq(ADDRINT address, UINT32 size, shmem_req_t shmem_req_type);

	//TODO rename this function (and others that interface with Network)
	void addMemRequest(NetPacket req_packet);
	void processSharedMemReq(NetPacket req_packet);
	void processUnexpectedSharedMemUpdate(NetPacket update_packet);

	static string sMemReqTypeToString(shmem_req_t type);
// these below functions have been pushed into initiateSharedMemReq (which directly calls ocache->runModel...)
//  bool runDCacheLoadModel(ADDRINT d_addr, UINT32 size);
//  bool runDCacheStoreModel(ADDRINT d_addr, UINT32 size);
    void debugSetDramState(ADDRINT addr, DramDirectoryEntry::dstate_t dstate, vector<UINT32> sharers_list);
	 bool debugAssertDramState(ADDRINT addr, DramDirectoryEntry::dstate_t dstate, vector<UINT32> sharers_list);

	 /* ============================================== */
	 /* Added by George */
	 void runDramAccessModel(void);
	 UINT64 getDramAccessCost(void);
	 /* ============================================== */

	/*
	 void setDramBoundaries(vector< pair<ADDRINT, ADDRINT> > addr_boundaries);
	*/
};
#endif
