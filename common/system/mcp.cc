#include "mcp.h"

MCP::MCP()
   :
      MCP_SERVER_MAX_BUFF(256*1024),
      scratch(new char[MCP_SERVER_MAX_BUFF]),
      syscall_server(pt_endpt, send_buff, recv_buff, MCP_SERVER_MAX_BUFF, scratch),
      sync_server(pt_endpt, recv_buff)
{
   pt_endpt.ptInitMCP();
}

MCP::~MCP()
{
   delete[] scratch;
}

void MCP::run()
{
//   cerr << "Waiting for MCP request..." << endl;

   send_buff.clear();
   recv_buff.clear();

   UInt32 length = 0;
   UInt8* buf = pt_endpt.ptMCPRecv(&length); 

   recv_buff << make_pair(buf, length);
  
   int msg_type;
   int comm_id;

   recv_buff >> msg_type >> comm_id;

   switch(msg_type)
   {
      case MCP_MESSAGE_SYS_CALL:
         syscall_server.handleSyscall(comm_id);
         break;
      case MCP_MESSAGE_QUIT:
         cerr << "Got the quit message... done waiting for MCP messages..." << endl;
         break;
      case MCP_MESSAGE_MUTEX_INIT:
         sync_server.mutexInit(comm_id); 
        break;
      case MCP_MESSAGE_MUTEX_LOCK:
         sync_server.mutexLock(comm_id);
         break;
      case MCP_MESSAGE_MUTEX_UNLOCK:
         sync_server.mutexUnlock(comm_id);
         break;
      case MCP_MESSAGE_COND_INIT:
         sync_server.condInit(comm_id);
         break;
      case MCP_MESSAGE_COND_WAIT:
         sync_server.condWait(comm_id);
         break;
      case MCP_MESSAGE_COND_SIGNAL:
         sync_server.condSignal(comm_id);
         break;
      case MCP_MESSAGE_COND_BROADCAST:
         sync_server.condBroadcast(comm_id);
         break;
      default:
         cerr << "Unhandled MCP message type: " << msg_type << " from: " << comm_id << endl;
         assert(false);
   }

   delete [] buf;

//   cerr << "Finished MCP request" << endl;
}

void MCP::finish()
{
   cerr << "Got finish request..." << endl;
   UnstructuredBuffer quit_buff;
   quit_buff.clear();

   int msg_type = MCP_MESSAGE_QUIT;
   int commid = -1;
   quit_buff << msg_type << commid;   

   cerr << "Sending message to MCP to quit..." << endl;
   pt_endpt.ptSendToMCP((UInt8 *) quit_buff.getBuffer(), quit_buff.size());
}


