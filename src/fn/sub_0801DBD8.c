#include "global.h"
typedef struct {
    unsigned count : 16;
    unsigned reserved : 16;
    void *values;
} Args;
extern volatile u32 gUnk_0300523C;
void sub_082279A8(int, int, int, int, int, int, int);
void sub_0822AFE8(int);
void Script_Call(int, Args *);
void sub_08033590(void);
void Task_Kill(void *);
int ResultTutorial_Close(u8 *p)
{
    int value;
    Args args;
    switch (p[0xaf]) {
        case 0:
            sub_082279A8(3, 4, 4, 4, 4, 0xffff, 0);
            sub_0822AFE8(5);
            ++p[0xaf];
            *(u16 *)(p + 0xaa) = 0;
            break;
        case 1:
            if (++*(s16 *)(p + 0xaa) > 31) {
                args.count = 0;
                args.values = &value;
                if (*(u16 *)(p + 0x7a0))
                    Script_Call(*(u16 *)(p + 0x7a0), &args);
                {
                    volatile u32 *flags = &gUnk_0300523C;
                    u32 mask;
                    mask = ~4;
                    *flags &= mask;
                }
                sub_08033590();
                Task_Kill(p);
                return 1;
            }
            break;
    }
    return 0;
}
