#include "global.h"
#define Actor_RecomputeScriptedValue sub_0811E844
struct Item {
    void *fn;
    u8 pad4;
    u8 kind;
    u8 pad6[0x14 - 6];
};
struct A {
    u8 pad0[0x54];
    u8 count;
    u8 pad55[7];
    s32 value5c;
    s32 value60;
    s32 value64;
    u8 pad68[0xD0 - 0x68];
    struct Item items[1];
    u8 padE4[0x51C - 0xE4];
    u16 base;
};
static inline s32 cnt(struct A *a)
{
    return a->count;
}
s32 sub_08249240(void *, s32, void *);
static inline int clamp(int x)
{
    if (x > 999)
        x = 999;
    else if (x < 0)
        x = 0;
    return x;
}
void Actor_RecomputeScriptedValue(struct A *a)
{
    s32 i;
    struct Item *items = a->items;
    struct Item *it;
    struct Item *it2;
    a->value60 = (*(&a))->base; /* address-of/deref keeps agbcc's pointer register assignment */
    i = 0;
    if (i < a->count) {
        it = items;
        it2 = it;
        do {
            if (it->kind == 6)
                a->value60 = sub_08249240(it2, a->value60, it->fn);
            it++;
            it2++;
            i++;
        } while (i < a->count);
    }
    a->value60 = clamp(a->value60);
    a->value5c = a->value60;
    a->value64 = a->value60 >> 1;
}
