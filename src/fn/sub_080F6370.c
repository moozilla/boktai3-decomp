#include "global.h"
// MATCHING_CONCESSION: register-pinning
void Callback_InvokeActor(void *, void *);
void Armor_UpdatePartColliders_080F2260(void *);
static inline u32 Flags(u8 *p, u32 mask)
{
    register u32 m asm("r1") = mask;
    register u32 bits asm("r0");
    asm("" : "+r"(m));
    bits = p[0x119];
    asm("" : "+r"(bits), "+r"(m));
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
        Armor_UpdatePartColliders_080F2260(p);
        callback = *(void **)(p + 0x168);
        if (callback)
            Callback_InvokeActor(p, callback);
    }
}
