#include "global.h"
struct Args08228368 {
    u16 count;
    u32 *values;
};
void *sub_0821A004(u32 list, u32 size); /* Task_Create variant (allocator sub_08219CB4) */
void Task_SetCallbacks(void *, void *, void *);
void sub_082282DC(u8 *);
void sub_08228364(u8 *);
/* Start a task that runs `script` after `delay` frames (a negative delay
 * sets flag 0x2C), with up to `args->count` copied argument words. */
u8 *sub_08228368(u32 script, struct Args08228368 *args, s32 delay, s32 urgent)
{
    u8 *t = sub_0821A004(urgent ? 3 : 5, 0x54);
    if (t) {
        if (!args)
            *(u16 *)(t + 0x1c) = 0;
        else {
            u32 *src = args->values;
            u32 *buf = (u32 *)(t + 0x30);
            u32 *dst;
            u16 n;
            *(u32 **)(t + 0x20) = buf;
            dst = buf;
            *(u16 *)(t + 0x1c) = args->count;
            for (n = args->count; n != 0; n--)
                *dst++ = *src++;
        }
        if (delay < 0) {
            *(u16 *)(t + 0x2c) = 1;
            delay = -delay;
        } else
            *(u16 *)(t + 0x2c) = 0;
        *(s32 *)(t + 0x28) = delay;
        *(u32 *)(t + 0x24) = script;
        Task_SetCallbacks(t, sub_082282DC, sub_08228364);
    }
    return t;
}
