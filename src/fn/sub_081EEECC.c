#include "global.h"
#include "contact_actor.h"
#include "engine.h"
struct DepartureOwner {
    u8 unknown0[0xAF];
    u8 complete, unknownB0, entry;
    u8 unknownB2[0x304 - 0xB2];
    u32 flag304;
    u8 unknown308[0x34C - 0x308];
    u32 flag34C;
    u8 unknown350[0x5E6 - 0x350];
    s16 shakeX, shakeZ;
};
struct PackedPosition {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
#define FIELD(p, n, t) (*(t *)((u8 *)(p) + (n)))
extern const u16 gUnk_0203B400[];
void Sound_PlaySong(u32);
void SpriteParticle_InitParams(void *);
void SpriteParticle_SetMotionParams(void *, u16, u16, u8, u32);
void SpriteParticle_SetSpriteResource(void *, u32, u32, s32);
void SpriteParticle_SetAnimationTrack(void *, void *, u32);
void SpriteParticle_SetPlanarVelocity(void *, s32, s32);
void SpriteParticle_Emit(void *, void *);
u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);
#ifdef NONMATCHING
// fuzzy: 65.5%
// fix.py best (sweep after the inlined-insn fix)
/* Remaining differences: receiver aliases, packed positions and particle stack allocation. */
static inline u32 Random(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return gUnk_0203B400[gRandomIndex];
}
static inline u32 Next(const u16 *table, u32 *randomIndex, u32 wrap)
{
    *randomIndex = (*randomIndex + 1) & wrap;
    return table[*randomIndex];
}
// confidence=low: shakes the two display axes and emits alternating particle tracks.
#define SingletonSlot7_UpdateShakingDeparture sub_081EEECC
void SingletonSlot7_UpdateShakingDeparture(struct DepartureOwner *p, s32 frame)
{
    u8 *actor = (u8 *)p;
    u32 k1 = 90;
    u32 k2;
    if (p->entry) {
        p->entry = 0;
        p->flag304 = 1;
        p->flag34C = 1;
        Sound_PlaySong(0x596);
    }
    p->shakeX = 0;
    p->shakeZ = 0;
    if (frame <= 59) {
        u32 index = (gRandomIndex + 1) & 1023;
        p->shakeX = (gUnk_0203B400[index] & 7) - 4;
        index = (index + 1) & 1023;
        gRandomIndex = index;
        p->shakeZ = (gUnk_0203B400[index] & 7) - 4;
    } else if (p->flag304 && p->flag34C)
        *((u32 *)(((u8 *)p) + 0x5A0)) += 2;
    if ((frame & 1) && (frame > 60)) {
        const u16 *table = gUnk_0203B400;
        u32 *randomIndex = &gRandomIndex;
        u32 wrap = 1023;
        struct PackedPosition position;
        s32 dx;
        s32 dz;
        u8 params[0x34];
        u32 index = ((*randomIndex) + 1) & wrap;
        position.x = ((table[index] & 31) + (*((u16 *)(((u8 *)actor) + 0x2DC)))) - 16;
        position.y = *((u16 *)(((u8 *)actor) + 0x2DE));
        position.z = 100;
        index = (index + 1) & wrap;
        dx = ((table[index] & 15) - 8) << 10;
        index = (index + 1) & wrap;
        *randomIndex = index;
        dz = -((s32)((table[index] & 7) << 10));
        SpriteParticle_InitParams(params);
        SpriteParticle_SetMotionParams(params, 0, 60, 0, 0);
        if (((s32)((u8)Next(table, randomIndex, wrap))) > 128) {
            SpriteParticle_SetSpriteResource(params, *((u32 *)(((u8 *)actor) + (k2 = 0x388))), 0,
                                             0x1F5);
            SpriteParticle_SetAnimationTrack(params, *((void **)(((u8 *)actor) + 0x38C)), 5);
        } else {
            SpriteParticle_SetSpriteResource(params, *((u32 *)(((u8 *)actor) + k2)), 0, 0x1F5);
            SpriteParticle_SetAnimationTrack(params, *((void **)(((u8 *)actor) + 0x38C)), 6);
        }
        SpriteParticle_SetPlanarVelocity(params, dx, dz);
        SpriteParticle_Emit(params, &position);
    }
    if ((*((s32 *)(((u8 *)actor) + 0x5A0))) == 58) {
        *((u16 *)(((u8 *)actor) + 0x5DC)) = k1;
        *((u16 *)(((u8 *)actor) + 0x5D8)) = *((u16 *)(((u8 *)actor) + 0x2DC));
        *((u16 *)(((u8 *)actor) + 0x5DA)) = 155;
        Sound_PlaySong(0x146);
        Sound_PlaySong(0x597);
    }
    if ((*((s32 *)(((u8 *)actor) + 0x5A0))) > 59)
        *((u32 *)(((u8 *)actor) + 0x5A0)) = 60;
    if ((frame > k1) && Sprite_BlinkUntilExpired(actor + 0x2C0, frame - k1, 45, k1))
        p->complete = 1;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081EEECC);
#endif
