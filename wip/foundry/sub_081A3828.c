#include "global.h"
/* Effect pool family names and field interpretations are tentative. */
#define EffectPool_ReleaseEntry sub_081A3828
#define gEffectPool             gUnk_0200023C
struct EffectPoolEntry {
    u32 flags;
    u8 unknown4[0xe8];
    u8 animation[0x6e];
    u8 childCount;
    u8 unknown15b[3];
    u16 state;
    u8 unknown160[8];
    void *children[4];
};
struct EffectPool {
    u8 unknown0[0x3c];
    u32 count;
    u8 unknown40[8];
    struct EffectPoolEntry entries[3];
};
extern struct EffectPool *gUnk_0200023C;
void sub_081A4834(void *, s32);
void sub_08013728(void *);
void sub_081A3828(u8 *entry)
{
    struct EffectPool *pool = gEffectPool;
    s32 i;
    void **child;
    void *animation;
    if (entry) {
        pool->count--;
        i = 0;
        for (; i < *(u8 *)(entry + 0x15a); i++) {
            sub_081A4834(((void **)(entry + 0x168))[i], 1);
        }
        *(u16 *)(entry + 0x15e) = 0;
        *(u32 *)entry |= 1;
        sub_08013728(entry + 0xec);
    }
}
