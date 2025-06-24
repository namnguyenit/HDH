#ifndef _GDT_H_
#define _GDT_H_

#include "types.h"

class GlobalDescriptorTable
{
public:
    GlobalDescriptorTable();
    ~GlobalDescriptorTable();

    // Gets the offset of code/data segments
    uint16_t CodeSegmentDescriptor();
    uint16_t DataSegmentDescriptor();

    class SegmentDescriptor
    {
    public:
        SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t flags);
        uint32_t Base();
        uint32_t Limit();

    private:
        uint16_t limit_low;
        uint8_t flags_limit_high;
        uint16_t base_low;
        uint8_t base_high;
        uint8_t type;
    } __attribute__((packed));


    SegmentDescriptor nullSegmentSelector;
    SegmentDescriptor unusedSegmentSelector;
    SegmentDescriptor codeSegmentSelector;
    SegmentDescriptor dataSegmentSelector;
    
};

#endif