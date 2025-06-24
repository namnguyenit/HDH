#include "types.h"
#include "gdt.h"
#include "interrupts.h"

void printf(char* str)
{
    // GPU reads from address 0xb8000 what to write to the screen
    // With variable type 'short' we group 2 bytes together
    // First byte is of background/foreground (4 bits each), and other byte for character
    uint16_t* VideoMemory = (uint16_t*)0xb8000;
    static uint8_t x = 0, y = 0;

    for (int i = 0; str[i] != '\0'; ++i)
    {
        // Go to the new row if you come to the new line character
        if (str[i] == '\n')
        {
            x = 0;
            ++y;
        }
        else 
        {
            // Set character to low-byte of the element and keep old value of high-byte
            VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xFF00) | str[i];
            ++x;
        }
        
        if (x >= 80) // We reached end of the line
        {
            ++y;
            x = 0;
        }

        if (y >= 25) // We reached end of the screen, clear it and start from beginning
        {
            for (y = 0; y < 25; ++y)
                for (x = 0; x < 80; ++x)
                    VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xFF00) | ' ';

            y = 0;
            x = 0;
        }
    }

}

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

extern "C" void callConstructors()
{
    for (constructor* i = &start_ctors; i != &end_ctors; ++i)
        (*i)();
}

extern "C" void kernelMain(void* multibootStruct, uint32_t magicNum)
{
    printf("Hello world\n");
    printf("This is my OS");

    GlobalDescriptorTable gdt;
    InterruptManager interrupts(&gdt);

    // Here init hardware

    interrupts.Activate();

    while (true);
}