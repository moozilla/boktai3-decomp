#include "global.h"
#define ActorAttachment_Create     sub_08237168
#define ActorAttachment_Initialize sub_08237108
#define ActorAttachment_Update     sub_082370AC
#define ActorAttachment_Destroy    sub_082370E8

void *Task_Create(u32, u32);
void Task_Kill(void *);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void sub_082370AC(void);
void sub_082370E8(void);
s32 sub_08237108(void *, void *, u32, void *, u32, u32, u32);

void *sub_08237168(void *parent, u32 kind, void *offset, u32 a, u32 b, u32 mode)
{
    void *p = Task_Create(8, 0x288);
    if (p) {
        Task_SetCallbacks(p, sub_082370AC, sub_082370E8);
        if (sub_08237108(p, parent, kind, offset, a, b, mode) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
