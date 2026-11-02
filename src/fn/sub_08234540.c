#include "spiral_burst.h"

#include "engine_globals.h"
#include "ram.h"

static inline u32 randomValue(const u16 *table, u32 mask)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & mask;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)table);
}

// MATCHING_CONCESSION: register-pinning
void SpiralBurst_InitParticles(struct SpiralBurst *burst)
{
    register u32 mask asm("sb");
    s32 i;
    u32 base;
    void *sprite;
    register const u16 *table asm("r5");
    register const u16 *randomTable asm("r6");
    struct SpiralParticleFrame *frame;
    {
        u32 next;
        table = gRandomTable;
        next = gRandomIndex + 1;
        mask = 1023;
        gRandomIndex = next & mask;
        base = (u8)table[gRandomIndex];
    }
    burst->resource = AnimData_FindEntry(0x1C1E);
    i = 0;
    randomTable = table;
    frame = &burst->particles[0].frame;
    sprite = burst->particles;
    for (; i < 8; i++) {
        ParticleSprite_Initialize(sprite, burst->resource, 0);
        Effect_SetOrigin(sprite, -4, -4);
        Effect_SetPalette(sprite, 1);
        frame->motion.angle = base + (i << 5) + (randomValue(randomTable, mask) & 15);
        frame->motion.riseRate = ((i & 3) << 1) + 3;
        frame->motion.radius = (randomValue(randomTable, mask) & 31) + 64;
        frame->motion.riseDelay = (randomValue(randomTable, mask) & 15) + 12;
        frame->motion.angularSpeed = 7;
        frame->motion.position.world.x =
            burst->origin.x +
            SpiralBurst_ScaleOffset(frame->motion.radius *
                                    gUnk_086149C4[(frame->motion.angle + 64) & 255]);
        frame->motion.position.world.y = burst->origin.y;
        {
            s32 radius = frame->motion.radius;
            const s16 *sn = gUnk_086149C4;
            asm("" : "+r"(sn));
            frame->motion.position.world.z =
                burst->origin.z + SpiralBurst_ScaleOffset(radius * sn[frame->motion.angle]);
        }
        ((struct SpiralParticle *)sprite)->renderMode = 2;
        frame->status = 0xEC;
        {
            u32 v = randomValue(randomTable, mask) & 7;
            frame->motion.animationFrame = v;
            if (v <= 3)
                SpriteDrawable_SelectTileFrame(sprite, burst->resource, 12);
            else
                SpriteDrawable_SelectTileFrame(sprite, burst->resource, 13);
        }
        frame->motion.lifetime = (randomValue(randomTable, mask) & 15) + 0x38;
        frame = (void *)((u8 *)frame + sizeof(struct SpiralParticle));
        sprite = (u8 *)sprite + sizeof(struct SpiralParticle);
    }
}
