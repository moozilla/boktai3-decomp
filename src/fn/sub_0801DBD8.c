#include "global.h"
#include "render.h"
typedef struct {
    unsigned count : 16;
    unsigned reserved : 16;
    void *values;
} Args;
void ScreenTransition_SendCommand(int, int, int, int, int, int, int);
void Sound_PauseChannel10(int);
void Script_Call(int, Args *);
void ScrollingDialogue_RequestHide(void);
void Task_Kill(void *);
int ResultTutorial_Close(u8 *p)
{
    int value;
    Args args;
    switch (p[0xaf]) {
        case 0:
            ScreenTransition_SendCommand(3, 4, 4, 4, 4, 0xffff, 0);
            Sound_PauseChannel10(5);
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
                    volatile u32 *flags = &gEnginePauseFlags;
                    u32 mask;
                    mask = ~4;
                    *flags &= mask;
                }
                ScrollingDialogue_RequestHide();
                Task_Kill(p);
                return 1;
            }
            break;
    }
    return 0;
}
