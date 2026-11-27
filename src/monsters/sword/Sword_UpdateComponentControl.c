#include "global.h"
void Callback_InvokeActor(void *, void *);
void Ivy_UpdatePartColliders(void *);
static inline u32 Flags(u8 *p, u32 mask)
{
    u32 m = mask;
    u32 bits;
    bits = p[0x119];
    return bits & m;
}
void Sword_UpdateComponentControl(u8 *p)
{
    void *callback;
    if (!(Flags(p, 16))) {
        if (*(u16 *)(p + 0xe0))
            --*(u16 *)(p + 0xe0);
        callback = *(void **)(p + 0x164);
        if (callback)
            Callback_InvokeActor(p, callback);
        Ivy_UpdatePartColliders(p);
        callback = *(void **)(p + 0x168);
        if (callback)
            Callback_InvokeActor(p, callback);
    }
}
