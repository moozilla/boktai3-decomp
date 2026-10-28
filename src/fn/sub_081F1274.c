// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "render.h"
#include "engine_globals.h"
#include "ram.h"
#define FIELD(p, n, t) (*(t *)((u8 *)(p) + (n)))
struct Slot7PackedPosition {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
extern void *gSceneCamera;
extern u8 *gParticipantActors;
extern u8 gUnk_03005040[];
void Collider_SetFixedPositionWithYOffset(void *, void *, s32);
void SpatialIndex_InsertEntry(void *);
void CpuSet(const void *, void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Palette_InterpolateBanks(void *, const void *, const void *, s32, u32);
void Anim_SetDurationScale(void *, u32);
s32 Div(s32, s32);
s32 Mod(s32, s32);
void Sound_PlaySong(u32);
void ScreenLayers_SelectRandomVariant(void *);
void ActorDisplay_RefreshScreenLayer(void *);
void SpriteParticle_InitParams(void *);
void SpriteParticle_SetMotionParams(void *, u32, u32, u32, u32);
void SpriteParticle_SetSpriteResource(void *, u32, u32, u32);
void SpriteParticle_SetAnimationTrack(void *, u32, u32);
void SpriteParticle_SetPlanarVelocity(void *, s32, s32);
void SpriteParticle_Emit(void *, void *);
u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
void Script_Call(u32, u32);
#ifdef NONMATCHING
// fuzzy: 51.5%
/* Remaining differences: palette-loop hoisting, packed particle storage and owner lifetimes. */
static inline u8 IsSceneHidden(void)
{
    if (gParticipantActors && (FIELD(gParticipantActors, 0x8F4, u16) & 1))
        return 1;
    return 0;
}
static inline u32 Random(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return gRandomTable[gRandomIndex];
}
static inline void CopyFourPalettes(u8 *source, u8 *dest)
{
    s32 i = 3;
    do {
        CpuSet(source, dest, 0x10);
        dest += 0x20;
        source += 0x20;
    } while (--i >= 0);
}
static inline void FlashPalette(u8 *actor)
{
    u8 *dest = ((u8 *)gBgPaletteBuffer) + 0x80;
    s32 i = 3;
    do {
        CpuSet(actor + 0x3A8, dest, 0x10);
        dest += 0x20;
    } while (--i >= 0);
    asm("" : "+r"(actor));
    FIELD(actor, 0x35C, void *) = actor + 0x3A8;
    FIELD(actor, 0x378, void *) = actor + 0x3A8;
    FIELD(actor, 0x5C2, u16)--;
    if (FIELD(actor, 0x5C2, u16) == 0) {
        CopyFourPalettes(actor + 0x508, ((u8 *)gBgPaletteBuffer) + 0x80);
        AnimData_SelectPaletteBank(actor + 0x350, FIELD(actor, 0x5FC, u32));
        AnimData_SelectPaletteBank(actor + 0x36C, FIELD(actor, 0x608, u32));
    }
    if (FIELD(actor, 0x5C2, u16) & 1)
        FIELD(actor, 0x5E0, s16) = FIELD(actor, 0x5C2, s16) >> 2;
    else
        FIELD(actor, 0x5E0, s16) = -(FIELD(actor, 0x5C2, s16) >> 2);
}
static inline void InterpolatePalettes(u8 *actor)
{
    s32 count = FIELD(actor, 0x5A8, s32);
    u32 shift = FIELD(actor, 0x5AC, u32);
    s32 limit = 1 << shift;
    if (count < limit) {
        s32 i;
        FIELD(actor, 0x5A8, u32) = ++count;
        Palette_InterpolateBanks(actor + 0x3C8, gPaletteBankData + FIELD(actor, 0x5F4, u32) * 32,
                                 gPaletteBankData + FIELD(actor, 0x5F8, u32) * 32, count, shift);
        FIELD(actor, 0x35C, void *) = actor + 0x3C8;
        Anim_SetDurationScale(actor + 0x2EC,
                              (u16)(Div(count * 192, 1 << FIELD(actor, 0x5AC, u32)) + 64));
        Palette_InterpolateBanks(actor + 0x3E8, gPaletteBankData + FIELD(actor, 0x600, u32) * 32,
                                 gPaletteBankData + FIELD(actor, 0x604, u32) * 32,
                                 FIELD(actor, 0x5A8, u32), FIELD(actor, 0x5AC, u32));
        FIELD(actor, 0x378, void *) = actor + 0x3E8;
        for (i = 0; i <= 3; ++i)
            Palette_InterpolateBanks(gUnk_03005040 + i * 32, FIELD(actor, 0x60C + i * 4, void *),
                                     FIELD(actor, 0x61C + i * 4, void *), FIELD(actor, 0x5A8, u32),
                                     FIELD(actor, 0x5AC, u32));
        if (FIELD(actor, 0x5A8, u32) == (u32)(1 << FIELD(actor, 0x5AC, u32)))
            CopyFourPalettes(gUnk_03005040, actor + 0x508);
        if (FIELD(actor, 0x5A8, u32) == 120)
            Sound_PlaySong(0x594);
    }
}
static inline void SetDepartureResource(u8 *actor, void *params, u32 id)
{
    if ((s32)(u8)Random() > 128) {
        SpriteParticle_SetSpriteResource(params, FIELD(actor, 0x388, u32), 0, id);
        SpriteParticle_SetAnimationTrack(params, FIELD(actor, 0x38C, u32), 5);
    } else {
        SpriteParticle_SetSpriteResource(params, FIELD(actor, 0x388, u32), 0, id);
        SpriteParticle_SetAnimationTrack(params, FIELD(actor, 0x38C, u32), 6);
    }
}
static inline void EmitDepartureParticle(u8 *actor, u32 zero)
{
    u8 params[0x34];
    struct Slot7PackedPosition position;
    s32 dx, dz;
    u32 index = (gRandomIndex + 1) & 1023;
    position.x = (gRandomTable[index] & 15) + FIELD(actor, 0x5D8, u16) - 8;
    index = (index + 1) & 1023;
    position.y = (gRandomTable[index] & 7) + FIELD(actor, 0x5DA, u16) - 12;
    position.z = 100;
    index = (index + 1) & 1023;
    dx = ((gRandomTable[index] & 15) - 8) << 10;
    index = (index + 1) & 1023;
    gRandomIndex = index;
    dz = -(s32)((gRandomTable[index] & 7) << 10);
    SpriteParticle_InitParams(params);
    SpriteParticle_SetMotionParams(params, 0, (Random() & 15) + 0x34, 0, zero);
    if (FIELD(actor, 0x5A6, s16) == 0) {
        SpriteParticle_SetSpriteResource(params, 0x4384, 0, 0x2A7);
        SpriteParticle_SetAnimationTrack(params, 0xF62B, 0);
    } else if (FIELD(actor, 0x5A6, s16) == 1) {
        SpriteParticle_SetSpriteResource(params, 0x61F9, 0, 0x2A6);
        SpriteParticle_SetAnimationTrack(params, 0x31F4, 8);
    } else if (FIELD(actor, 0x5A6, s16) == 2) {
        if ((s32)(u8)Random() > 128) {
            SpriteParticle_SetMotionParams(params, 0, 42, 0, zero);
            SetDepartureResource(actor, params, 0x1F5);
        } else {
            SpriteParticle_SetSpriteResource(params, 0x61F9, 0, 0x1F5);
            SpriteParticle_SetAnimationTrack(params, 0x31F4, 8);
        }
    } else {
        index = (gRandomIndex + 1) & 1023;
        position.x = (gRandomTable[index] & 15) + FIELD(actor, 0x324, u16) - 8;
        index = (index + 1) & 1023;
        gRandomIndex = index;
        position.y = (gRandomTable[index] & 7) + FIELD(actor, 0x326, u16);
        position.z = 100;
        SpriteParticle_SetMotionParams(params, 0, 42, 0, zero);
        SetDepartureResource(actor, params, 2);
    }
    SpriteParticle_SetPlanarVelocity(params, dx, dz);
    SpriteParticle_Emit(params, &position);
}
// confidence=low: updates slot-seven collision, palette transitions, particles and damage feedback.
#define SingletonSlot7_UpdatePresentation sub_081F1274
void SingletonSlot7_UpdatePresentation(u8 *actor)
{
    u8 mode;
    u32 hidden;
    if (FIELD(actor, 0x5D0, u16)) {
        FIELD(actor, 0x9C, u32) |= 4;
        FIELD(actor, 0x5D0, u16)--;
        if (FIELD(actor, 0x5D0, u16) == 0)
            FIELD(actor, 0x9C, u32) &= ~4;
    }
    if (gSceneCamera) {
        Collider_SetFixedPositionWithYOffset(actor + 0x17C, actor + 0x268, 0);
        FIELD(actor, 0x590, u32) = 0;
        FIELD(actor, 0x1BC, u32) &= ~1;
        Collider_SetFixedPositionWithYOffset(actor + 0x1BC, actor + 0x268, 0);
        SpatialIndex_InsertEntry(actor + 0x1BC);
    }
    hidden = IsSceneHidden();
    mode = actor[0xB3];
    if (hidden == 1 && mode == 0)
        FIELD(actor, 0x17C, u32) |= 1;
    else
        FIELD(actor, 0x17C, u32) &= ~1;
    if (mode) {
        if (FIELD(actor, 0x5C2, s16) > 0)
            FlashPalette(actor);
        else {
            FIELD(actor, 0x5E0, u16) = 0;
            InterpolatePalettes(actor);
        }
    } else if (FIELD(actor, 0x5C2, s16) > 0)
        FlashPalette(actor);
    if (actor[0xB3]) {
        ScreenLayers_SelectRandomVariant(actor);
        ActorDisplay_RefreshScreenLayer(actor);
        if (FIELD(actor, 0x5DC, s16) > 0) {
            u32 remainder = Mod(FIELD(actor, 0x90, u32), 6);
            if (!remainder)
                EmitDepartureParticle(actor, remainder);
            FIELD(actor, 0x5DC, u16)--;
        }
    }
    if (FIELD(actor, 0x5EC, s16) != 0) {
        FIELD(actor, 0x5EA, u16)++;
        Sprite_BlinkUntilExpired(actor + 0x308, FIELD(actor, 0x5EA, s16), 45, 90);
    }
    if (FIELD(actor, 0x5C0, s16) > 0 || FIELD(actor, 0x5C2, s16) != 0) {
        FIELD(actor, 0x94, u16) -= FIELD(actor, 0x5C0, u16);
        if (FIELD(actor, 0x5C0, s16) > 0) {
            struct Slot7PackedPosition color;
            color.x = 32;
            color.y = 0;
            color.z = 32;
            DamageNumberDisplay_Show(actor + 0x1FC, FIELD(actor, 0x5C0, s16), 0, actor + 0x260,
                                     &color, 1);
            if ((s32)(u8)Random() > 196)
                Sound_PlaySong(0x587);
            else
                Sound_PlaySong(0x13E);
        }
        if (FIELD(actor, 0x94, s16) <= 0 && FIELD(actor, 0x5E2, s16) == 0 && actor[0xB3] == 1) {
            CopyFourPalettes(actor + 0x508, ((u8 *)gBgPaletteBuffer) + 0x80);
            Script_Call(FIELD(actor, 0x588, u32), 0);
            FIELD(actor, 0x5E2, u16) = 1;
        }
    }
    if (FIELD(actor, 0x240, u16))
        FIELD(actor, 0x240, u16)--;
    if (actor[0xB3] == 0 && FIELD(actor, 0x1AE, u16)) {
        FIELD(actor, 0x1AE, u16)--;
        if (FIELD(actor, 0x1AE, u16) == 0)
            actor[0x18A] = 0;
    }
    FIELD(actor, 0x5C0, u16) = 0;
    FIELD(actor, 0xE4, u32)++;
    FIELD(actor, 0xBC, u32) = 0;
    FIELD(actor, 0xC0, u32) = 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081F1274);
#endif
