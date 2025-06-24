#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#include "types.h"
#include "gdt.h"
#include "port.h"

class InterruptManager
{
public:
    InterruptManager(GlobalDescriptorTable* gdt);
    ~InterruptManager();

    static uint32_t HandleInterrupt(uint8_t interruptNum, uint32_t esp);
    static void IgnoreInterruptRequest();
    static void HandleInterruptRequest0x00(); // Timer interrupt
    static void HandleInterruptRequest0x01(); // Keyboard interrupt

    void Activate();

    Port8BitSlow picMasterCommand;
    Port8BitSlow picSlaveCommand;
    Port8BitSlow picMasterData;
    Port8BitSlow picSlaveData;

protected:

    struct GateDescriptor
    {
        uint16_t handlerAddressLowBits;
        uint16_t gdtCodeSegmentSelector;
        uint8_t reserved;
        uint8_t access;
        uint16_t handlerAddressHighBits;
    } __attribute__((packed));

    struct InterruptDescriptorTablePointer
    {
        uint16_t size;
        uint32_t base;
    } __attribute__((packed));

    static GateDescriptor interruptDescriptorTable[256];

    static void SetInterruptDescriptorTableEntry(
        uint8_t interruptNum,
        uint16_t codeSegmentSelectorOffset,
        void (*handler)(),
        uint8_t descriptorPrivilegeLevel,
        uint8_t descriptorType
    );
};

#endif