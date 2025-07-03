#include "types.h"
#include "gdt.h"
#include "interrupts.h"

// Hàm đọc 1 byte từ cổng I/O (chuẩn x86)
static inline uint8_t inb(uint16_t port) {
    uint8_t result;
    asm volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Biến vị trí con trỏ toàn cục cho file
static uint8_t x = 0, y = 0;

void printf(const char* str)
{
    uint16_t* VideoMemory = (uint16_t*)0xb8000;

    for (int i = 0; str[i] != '\0'; ++i)
    {
        if (str[i] == '\n')
        {
            x = 0;
            ++y;
        }
        else 
        {
            VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xFF00) | str[i];
            ++x;
        }
        
        if (x >= 80)
        {
            ++y;
            x = 0;
        }

        if (y >= 25)
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


// Scan code cho '-' là 0x0C
const char scancode_to_ascii[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-',0,8,0,      // 0x00 - 0x0F
    'q','w','e','r','t','y','u','i','o','p','[',']','\n',0,'a','s', // 0x10 - 0x1F ('y' at 0x15)
    'd','f','g','h','j','k','l',';',0,0,0,'\\','z','x','c','v',     // 0x20 - 0x2F
    'b','n','m',',','.','/',0,0,0,' ',0,0,0,0,0,0,                 // 0x30 - 0x3F ('n' at 0x31)
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                            // 0x40 - 0x4F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                            // 0x50 - 0x5F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                            // 0x60 - 0x6F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0                             // 0x70 - 0x7F
};

char getch() {
    char c = 0;
    while (c == 0) {
        if ((inb(0x64) & 1) != 0) {
            uint8_t sc = inb(0x60);
            if (sc < 128) {
                c = scancode_to_ascii[sc];
            }
        }
    }
    return c;
}


int input_number() {
    char buffer[16];
    int i = 0;
    while (1) {
        char c = getch();

        if (c == '\n') {
            buffer[i] = '\0';
            printf("\n");
            break;
        } 
        else if (c == 8) { 
            if (i > 0) {
                i--;
                x--;
                uint16_t* VideoMemory = (uint16_t*)0xb8000;
                VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xFF00) | ' ';
            }
        }
        else if (c == '-' && i == 0) { 
            buffer[i++] = c;
            char s[2] = {c, 0};
            printf(s);
        }
        else if (c >= '0' && c <= '9' && i < 15) { 
            buffer[i++] = c;
            char s[2] = {c, 0};
            printf(s);
        }
    }

    int num = 0;
    int start_index = 0;
    int is_negative = 0;

    if (buffer[0] == '-') {
        is_negative = 1;
        start_index = 1;
    }

    for (int j = start_index; buffer[j] != '\0'; ++j) {
        num = num * 10 + (buffer[j] - '0');
    }

    if (is_negative) {
        num = -num;
    }

    return num;
}


void printf_int(int num) {
    char out[16];
    int idx = 15;
    out[idx--] = '\0';
    if (num == 0) out[idx--] = '0';
    int is_negative = 0;
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }
    while (num > 0 && idx >= 0) {
        out[idx--] = '0' + (num % 10);
        num /= 10;
    }
    if (is_negative && idx >= 0) out[idx--] = '-';
    printf(&out[idx+1]);
}

uint32_t isqrt(uint32_t n) {
    if (n == 0) return 0;
    uint32_t low = 1;
    uint32_t high = n;
    uint32_t result = 1;
    while (low <= high) {
        uint32_t mid = low + (high - low) / 2;
        if (mid > n / mid) {
            high = mid - 1;
        } else {
            result = mid;
            low = mid + 1;
        }
    }
    return result;
}

void clear_screen()
{
    // Con trỏ tới bộ nhớ video
    uint16_t* VideoMemory = (uint16_t*)0xb8000;
    for (y = 0; y < 25; ++y)
    {
        for (x = 0; x < 80; ++x)
        {
            // Ghi một khoảng trắng với màu mặc định
            VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xFF00) | ' ';
        }
    }
    // Đưa con trỏ về vị trí đầu màn hình
    x = 0;
    y = 0;
}


extern "C" void kernelMain(void* multibootStruct, uint32_t magicNum)
{
    GlobalDescriptorTable gdt;
    InterruptManager interrupts(&gdt);

    
    while(true)
    {
        
        clear_screen();
        printf("+===============================================================+\n");
        printf(" | HELLO WORLD!                                                |\n");
        printf(" | THIS MY OS                                                  |\n");
        printf(" | Nhom 12                                                     |\n");
        printf(" | Student at Faculty of Computer Science, Phenikaa University |\n");
        printf("+===============================================================+\n");
        
        printf("he so a: ");
        int a = input_number();

        printf("he so b: ");
        int b = input_number();

        printf("he so c: ");
        int c = input_number();

        printf("Phuong trinh: ");
        printf_int(a);
        printf("x^2 + ");
        printf_int(b);
        printf("x + ");
        printf_int(c);
        printf(" = 0\n");

        if (a == 0) {
            printf("phuong trinh bac nhat.\n");
            if (b == 0) {
                if (c == 0) {
                    printf("phuong trinh co vo so nghiem.\n");
                } else {
                    printf("phuong trinh vo nghiem.\n");
                }
            } else {
                printf("phuong trinh co mot nghiem:\nx = ");
                if ((-c) % b == 0) {
                    printf_int((-c) / b);
                } else {
                    printf_int(-c);
                    printf("/");
                    printf_int(b);
                }
                printf("\n");
            }
        } else {
            int delta = b*b - 4*a*c;
            printf("Delta = ");
            printf_int(delta);
            printf("\n");

            if (delta < 0) {
                printf("Phuong trinh vo nghiem.\n");
            } else if (delta == 0) {
                printf("Phuong trinh co nghiem kep:\nx = ");
                if ((-b) % (2*a) == 0) {
                    printf_int((-b) / (2*a));
                } else {
                    printf_int(-b);
                    printf("/");
                    printf_int(2*a);
                }
                printf("\n");
            } else {
                uint32_t sqrt_delta = isqrt(delta);
                printf("Phuong trinh co 2 nghiem nguyen phan biet:\n");
                
                printf("x1 = ");
                if ((-b + sqrt_delta) % (2*a) == 0) {
                    printf_int((-b + (int)sqrt_delta) / (2*a));
                } else {
                    printf("(");
                    printf_int(-b);
                    printf(" + ");
                    printf_int(sqrt_delta);
                    printf(") / ");
                    printf_int(2*a);
                }
                printf("\n");
                
                printf("x2 = ");
                if ((-b - (int)sqrt_delta) % (2*a) == 0) {
                    printf_int((-b - (int)sqrt_delta) / (2*a));
                } else {
                    printf("(");
                    printf_int(-b);
                    printf(" - ");
                    printf_int(sqrt_delta);
                    printf(") / ");
                    printf_int(2*a);
                }
                printf("\n");
            }
        }
        


        
        printf("\nBan co muon tiep tuc khong? (y/n): ");
        char choice = 0;
        while(true)
        {
            choice = getch();
            if(choice == 'y' || choice == 'Y' || choice == 'n' || choice == 'N')
            {
                // In ký tự người dùng vừa gõ ra màn hình
                char s[2] = {choice, '\0'};
                printf(s);
                break;
            }
        }

        if(choice == 'n' || choice == 'N')
        {
            break; 
        }

    }
    clear_screen();
    interrupts.Activate();
    while (true);

}