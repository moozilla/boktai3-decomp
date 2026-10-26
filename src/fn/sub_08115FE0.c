// Tentative family and semantic names; layouts follow observed field offsets.
#include "global.h"
#include "ram.h"
struct CompactParticleSlot {
    u8 animation[0x1c];
    u32 spriteFlags;
    u8 spriteUnknown[3];
    u8 spriteMode;
    u8 spriteTail[0x14];
    u8 motion[8];
    u8 unknown40[0x18];
    void *script;
    u8 unknown5C[0x54];
    u8 state;
    u8 tail[7];
};
struct CompactParticlePool {
    u8 unknown[0x18];
    struct CompactParticleSlot slots[16];
};
s32 AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void *ScriptProc_Start(u16, u16);
void Memory_Clear(void *, u32);
s32 ActorParticles_InitCompactPool(struct CompactParticlePool *pool)
{
    s32 i;
    for (i = 0; i < 16; i++) {
        if (!AnimData_Load(pool->slots[i].animation, 0xae8e))
            return -1;
        Sprite_Init(&pool->slots[i].spriteFlags, pool->slots[i].animation, 0);
        pool->slots[i].script = ScriptProc_Start(0x922e, 0x87e6);
        Memory_Clear(pool->slots[i].motion, 8);
        pool->slots[i].state = 0;
        pool->slots[i].spriteFlags |= 0x11;
        pool->slots[i].spriteMode = 2;
    }
    gCompactParticlePool = pool;
    return 0;
}
