#ifndef __PIN_CONFIG_H__
#define __PIN_CONFIG_H__

class PinConfig
{
public:
   class StackAttributes
   {
   public:
      IntPtr lower_limit;
      UInt32 size;
   };

   // Since Pin messes with stack, we need to handle that separately
   UInt32 getStackLowerLimit() const
   { return m_stack_lower_limit; }
   UInt32 getStackUpperLimit() const
   { return m_stack_upper_limit; }
   UInt32 getStackSizePerCore() const
   { return m_stack_size_per_core; }

private:
   // Pin specific variables
   static UInt32 m_current_process_num;
   static UInt32 m_total_cores;
   static UInt32 m_num_local_cores;
   
   static IntPtr m_stack_lower_limit;
   static UInt32 m_stack_size_per_core;
   static IntPtr m_stack_upper_limit;

   void setStackBoundaries();
};

#endif /* __PIN_CONFIG_H__ */
