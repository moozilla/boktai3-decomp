#include "global.h"
#include "contact_actor.h"
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
struct PackedPosition { s32 x:16, y:16, z:16, pad:16; };
#define FIELD(p,n,t) (*(t *)((u8 *)(p)+(n)))
extern const u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
void Sound_PlaySong(u32);
void SpriteParticle_InitParams(void *);
void SpriteParticle_SetMotionParams(void *, u16, u16, u8, u32);
void SpriteParticle_SetSpriteResource(void *, u32, u32, s32);
void SpriteParticle_SetAnimationTrack(void *, void *, u32);
void SpriteParticle_SetPlanarVelocity(void *, s32, s32);
void SpriteParticle_Emit(void *, void *);
u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);
#ifdef NONMATCHING
// fuzzy: 66.2%
/* Remaining differences: receiver aliases, packed positions and particle stack allocation. */
static inline u32 Random(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return gUnk_0203B400[gUnk_03005308];
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
    if (p->entry) {
        p->entry = 0;
        p->flag304 = 1;
        p->flag34C = 1;
        Sound_PlaySong(0x596);
    }
    p->shakeX = 0;
    p->shakeZ = 0;
    if (frame <= 59) {
        u32 index = (gUnk_03005308 + 1) & 1023;
        p->shakeX = (gUnk_0203B400[index] & 7) - 4;
        index = (index + 1) & 1023;
        gUnk_03005308 = index;
        p->shakeZ = (gUnk_0203B400[index] & 7) - 4;
    } else if (p->flag304 && p->flag34C) FIELD(p,0x5A0,u32) += 2;
    if ((frame & 1) && frame > 60) {
        const u16 *table = gUnk_0203B400;
        u32 *randomIndex = &gUnk_03005308;
        u32 wrap = 1023;
        struct PackedPosition position;
        s32 dx, dz;
        u8 params[0x34];
        u32 index = (*randomIndex + 1) & wrap;
        position.x = (table[index] & 31) + FIELD(actor,0x2DC,u16) - 16;
        position.y = FIELD(actor,0x2DE,u16);
        position.z = 100;
        index = (index + 1) & wrap;
        dx = ((table[index] & 15) - 8) << 10;
        index = (index + 1) & wrap;
        *randomIndex = index;
        dz = -(s32)((table[index] & 7) << 10);
        SpriteParticle_InitParams(params);
        SpriteParticle_SetMotionParams(params, 0, 60, 0, 0);
        if ((s32)(u8)Next(table, randomIndex, wrap) > 128) {
            SpriteParticle_SetSpriteResource(params, FIELD(actor,0x388,u32), 0, 0x1F5);
            SpriteParticle_SetAnimationTrack(params, FIELD(actor,0x38C,void *), 5);
        } else {
            SpriteParticle_SetSpriteResource(params, FIELD(actor,0x388,u32), 0, 0x1F5);
            SpriteParticle_SetAnimationTrack(params, FIELD(actor,0x38C,void *), 6);
        }
        SpriteParticle_SetPlanarVelocity(params, dx, dz);
        SpriteParticle_Emit(params, &position);
    }
    if (FIELD(actor,0x5A0,s32) == 58) {
        FIELD(actor,0x5DC,u16) = 90;
        FIELD(actor,0x5D8,u16) = FIELD(actor,0x2DC,u16);
        FIELD(actor,0x5DA,u16) = 155;
        Sound_PlaySong(0x146);
        Sound_PlaySong(0x597);
    }
    if (FIELD(actor,0x5A0,s32) > 59) FIELD(actor,0x5A0,u32) = 60;
    if (frame > 90 && Sprite_BlinkUntilExpired(actor + 0x2C0, frame - 90, 45, 90)) p->complete = 1;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081EEECC);
#endif
