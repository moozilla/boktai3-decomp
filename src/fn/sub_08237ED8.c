#include "global.h"
#include "player.h"
void Task_Kill(void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_Refresh(void *);
u16 Terrain_GetHeightAt(void *);
struct ImpactSprite {
    u32 flags;
    u8 unknown4[12];
    u16 frame;
};
struct OrbitParticle {
    u8 unknown0[0x18];
    u32 spriteFlags;
    u8 unknown1C[0xC];
    u16 frame;
    u8 unknown2A[0xA];
    s16 x, y, z;
    u8 unknown3A[0x7A];
    s16 vx;
    u8 unknownB6[2];
    s16 vz;
    u8 unknownBA[2];
    u32 phase;
    u16 variant, timer;
};
// confidence=low: moving collider stops on terrain, then plays a short variant-dependent impact.
#define PlayerOrbitEffect_UpdateMovingParticle sub_08237ED8
s32 PlayerOrbitEffect_UpdateMovingParticle(struct OrbitParticle *p)
{
    if (p->phase) {
        u32 timer = ++p->timer;
        if (timer == 1)
            p->frame = 4;
        else if (timer == 4) {
            u16 *source = &p->variant;
            struct ImpactSprite *sprite = (void *)((u8 *)p + 0x18);
            s32 variant = *source;
            if (variant & 1)
                sprite->frame = 7;
            else if ((variant >> 1) & 1)
                sprite->frame = 8;
            else
                sprite->frame = 6;
            if ((s32)variant <= 2)
                sprite->flags &= ~12;
            else if ((s32)variant <= 4)
                sprite->flags = (sprite->flags & ~4) | 8;
            else if ((s32)variant <= 5)
                sprite->flags |= 12;
            else
                sprite->flags = (sprite->flags | 4) & ~8;
        } else if (timer > 6)
            Task_Kill(p);
    } else {
        p->x += p->vx;
        p->z += p->vz;
        Collider_SetPosition((u8 *)p + 0x60, &p->x, 0);
        Collider_Refresh((u8 *)p + 0x60);
        {
            u32 *flags = (u32 *)(((u8 **)gPlayers)[0] + 0x20);
            u32 bits = 0x80002;
            *flags |= bits;
        }
        if (Terrain_GetHeightAt(&p->x) > p->y) {
            p->phase = 1;
            p->timer = 0;
        } else if (++p->timer > 59)
            Task_Kill(p);
    }
    return 0;
}
