#include "global.h"
#include "engine.h"
struct SpawnOffset {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
};
extern const u16 gUnk_0203B400[];
void Player_FlushQueuedEffects(u8 *);
s32 EffectGroup_Spawn(void *, u32, u32, struct SpawnOffset *);
void Sound_PlaySong(u32);

void sub_0815A1AC(u8 *p, u32 kind, s32 count)
{
    struct SpawnOffset off;
    s32 i, spawned;
    if (!(gEngineFlags & 0x1000))
        return;
    if (p[0x5bb] != 0) {
        u8 cur = p[0x5b9];
        if (cur != kind) {
            if (cur == 3)
                return;
            Player_FlushQueuedEffects(p);
        }
    }
    if (p[0x5bb] == 0) {
        u8 t = p[0x4f4];
        u8 *dst = p + 0x4f5;
        u16 zero = 0;
        *dst = t;
        *(u16 *)(p + 0x4c6) = zero;
    }
    {
        u16 index;
        u32 mask;
        u32 *indexPtr;
        const u16 *table;
        table = gUnk_0203B400;
        indexPtr = &gRandomIndex;
        index = *indexPtr + 1;
        mask = 0x3ff;
        index &= mask;
        off.x = (table[index] % 16) - 7;
        index = (index + 1) & mask;
        *indexPtr = index;
        off.y = -(table[index] % 32);
        off.z = 0;
    }

    spawned = 0;
    for (i = 0; i < count; i++) {
        if (EffectGroup_Spawn(p + 0x5b8, kind, 0xe10, &off) >= 0)
            spawned = 1;
    }
    if (spawned) {
        if (p[0x5b9] == 1)
            Sound_PlaySong(0x191);
        else if (p[0x5b9] == 2) {
            *(u16 *)(p + 0x442) += 10;
            Sound_PlaySong(0x133);
        } else if (p[0x5b9] == 3) {
            Sound_PlaySong(0x133);
            *(u16 *)(p + 0x270) = 10;
            if (*(u16 *)(p + 0x274) == 0)
                *(u16 *)(p + 0x274) = 1;
            p[0x4ba] = 0;
        }
    }
    *(u16 *)(p + 0x530) = 0;
}
