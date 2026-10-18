#include "global.h"

extern const u8 gUnk_0824E224[];
void Vorn_SelectRecoveryAction(u8 *, const u8 *, u32);

void Vorn_UpdateActionCompletion(u8 *p)
{
    if (p[0x2b1])
        p[0x2b1] = 0;
    if (p[0x2b0]) {
        struct {
            u8 pad[0xf];
            u8 flags;
        } *sprite = (void *)(p + 0xdc);
        u8 m = 8;
        sprite->flags |= m;
        Vorn_SelectRecoveryAction(p, gUnk_0824E224, 0x19b);
    } else
        (*(u32 *)(p + 0x2ac))++;
}
