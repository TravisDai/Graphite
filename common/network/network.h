// Harshad Kasture
//

#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>
#include <sched.h>
#include <unistd.h>
#include <queue>

// JME: not entirely sure why this is needed...
class Chip;

#include "chip.h"
#include "transport.h"


// Define data types

// enums for type of network packet
enum PacketType 
{
   MIN_PACKET_TYPE = 0, 
   INVALID = MIN_PACKET_TYPE, 
   USER,
   MAX_PACKET_TYPE = USER
};

// network packet
typedef struct NetPacket
{
   PacketType type;
   int sender;
   int receiver;
   unsigned int length;
   char *data;
} NetPacket;


// network query struct
typedef struct NetMatch
{
   int sender;
   bool sender_flag;
   PacketType type;
   bool type_flag;
} NetMatch;


class Network{

   private:

      typedef struct NetQueueEntry{
         NetPacket packet;
	 UINT64 time;
      } NetQueueEntry;
      
      class earlier{
         public:
	    bool operator() (const NetQueueEntry& first, \
            const NetQueueEntry& second) const
	    {
	       return first.time <= second.time;
	    }
      };
		
      typedef priority_queue <NetQueueEntry, vector<NetQueueEntry>, earlier>
              NetQueue;

      Chip *the_chip;		
      int net_tid;
      int net_num_mod;
      char* netCreateBuf(NetPacket packet);
      void netExPacket(char* buffer, NetPacket &packet, UINT64 &time);
      NetQueue **net_queue;
      Transport *transport;

   public:

      int netInit(Chip *chip, int tid, int num_threads);
      int netSend(NetPacket packet);
      NetPacket netRecv(NetMatch match);
      bool netQuery(NetMatch match);
		
};

#endif