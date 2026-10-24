#include "global.h"
// MATCHING_CONCESSION: register-pinning

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
struct Vec16 {
    s16 x, y, z;
};
#define U8(p, o)  (*(u8 *)((u8 *)(p) + (o)))
#define S8(p, o)  (*(s8 *)((u8 *)(p) + (o)))
#define U16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S16(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define U32(p, o) (*(u32 *)((u8 *)(p) + (o)))
#define S32(p, o) (*(s32 *)((u8 *)(p) + (o)))
extern u32 gUnk_030053F4, gUnk_030053F0;
extern u8 *gUnk_020004B4, *gUnk_02000710;
extern s32 gUnk_020004E0[];
extern const s16 gUnk_086149C4[];
void Character_GetHeightAdjustedPosition(void *, struct Vec16 *);
void SpatialIndex_SetPosition(void *, s32, s32, s32);
void SpatialIndex_InsertEntry(void *);
void PaletteFade_SetScrollPosition(s32, s32);
void ActorStats_ApplyPendingDamage(void *);
void Character_UpdateTimedEffect(void *);
void ActorParticles_UpdateEmitter(void *, void *, u32);
void CharacterModel_SetAnimationFlags(void *, u32);
void CharacterModel_ClearAnimationFlags(void *, u32);
void ActorMarkerPool_HideMarker(s32);
void ActorMarkerPool_ShowMarker(s32);
s32 ModifierStats_ApplyKind11(void *, s32);
s32 ModifierStats_ApplyKinds2And5(void *, s32);
s32 Div(s32, s32);
static inline s32 Scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -(-v >> 12);
}
static inline u32 LinkMode(u32 mask)
{
    return (gUnk_030053F4 | gUnk_030053F0) & mask;
}
#define Flag(p, mask) ((p) && (U16(p, 0x8F4) & (mask)))
static inline u8 Inactive(u8 *p)
{
    if (p) {
        if (U16(p, 0x8F4) & 1)
            return 1;
    }
    return 0;
}
static inline s32 LocalParticipant(void)
{
    u8 *p = gUnk_020004B4;
    if (!p)
        return -1;
    return S16(p, 0x18);
}
static inline s32 ParticipantStat(s32 slot)
{
    u8 *p = gUnk_020004B4;
    if (!p || slot < 0)
        return 0;
    return p[0x5C + slot];
}
// confidence=low: updates spatial entries, local presentation and resource recovery.
#define Glove_UpdatePostState sub_0811A158
void Glove_UpdatePostState(u8 *p)
{
    u8 *self = p + 0x18;
    if (!Inactive(p)) {
        struct Vec16 pos;
        u32 *motion;
        Character_GetHeightAdjustedPosition(self, &pos);
        SpatialIndex_SetPosition(self + 0x56C, pos.x, pos.y, pos.z);
        motion = (u32 *)(self + 0x56C);
        if ((*((s16 *)(((u8 *)self) + 0x5F0))) < 0) {
            SpatialIndex_SetPosition(self + 0x52C, pos.x, pos.y, pos.z);
        } else {
            struct Vec16 off;
            struct Vec16 *o = &off;
            s32 angle = *((s16 *)(((u8 *)self) + 0x5F0));
            s32 radius = 20;
            o->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
            o->y = 0;
            o->z = Scale(gUnk_086149C4[angle & 255] * radius);
            SpatialIndex_SetPosition(self + 0x52C, pos.x + off.x, pos.y, pos.z + off.z);
        }
        if (*((u16 *)(((u8 *)self) + 0x59E))) {
            (*((u16 *)(((u8 *)self) + 0x59E)))--;
            *motion |= 1;
        } else
            *motion &= ~1;
        SpatialIndex_InsertEntry(self + 0x52C);
    }
    {
        register u8 *view asm("r6") = p + 0x18;
        if (LinkMode(0x800)) {
            if ((*((s8 *)(((u8 *)p) + 0x8F8))) == LocalParticipant()) {
                gUnk_020004E0[0] =
                    ((*((s32 *)(((u8 *)view) + 0x10))) - (*((s32 *)(((u8 *)view) + 0x5DC)))) >> 12;
                gUnk_020004E0[2] = (*((s32 *)(((u8 *)view) + 0x18))) >> 12;
                if (p && ((*((u16 *)(((u8 *)p) + 0x8F4))) & 4))
                    PaletteFade_SetScrollPosition(0, 0);
                else
                    PaletteFade_SetScrollPosition((*((s32 *)(((u8 *)p) + 0x8E8))) >> 12, 0);
            }
        } else {
            gUnk_020004E0[0] =
                ((*((s32 *)(((u8 *)view) + 0x10))) - (*((s32 *)(((u8 *)p) + 0x5F4)))) >> 12;
            gUnk_020004E0[2] = (*((s32 *)(((u8 *)view) + 0x18))) >> 12;
            if (p && ((*((u16 *)(((u8 *)p) + 0x8F4))) & 4))
                PaletteFade_SetScrollPosition(0, 0);
            else
                PaletteFade_SetScrollPosition((*((s32 *)(((u8 *)p) + 0x8E8))) >> 12, 0);
        }
        if (!(p && ((*((u16 *)(((u8 *)p) + 0x8F4))) & 0x14)))
            ActorStats_ApplyPendingDamage(self);
        Character_UpdateTimedEffect(self + 0x68);
        if ((!Inactive(p)) && (!(p && ((*((u16 *)(((u8 *)p) + 0x8F4))) & 0x10))))
            ActorParticles_UpdateEmitter(self, self + 0x44, *((u32 *)(((u8 *)self) + 0x5C)));
        if (p && ((*((u16 *)(((u8 *)p) + 0x8F4))) & 1)) {
            *((u32 *)(((u8 *)p) + 0x8FC)) = 0;
            *((u16 *)(((u8 *)p) + 0x33E)) = 0;
        } else if (p && ((*((u16 *)(((u8 *)p) + 0x8F4))) & 2)) {
            if (LinkMode(0x800)) {
                if ((*((s8 *)(((u8 *)p) + 0x8F8))) == LocalParticipant()) {
                    CharacterModel_SetAnimationFlags(p + 0x610, 0x200);
                } else {
                    CharacterModel_SetAnimationFlags(p + 0x610, 1);
                    ActorMarkerPool_HideMarker(*((s8 *)(((u8 *)p) + 0x860)));
                }
            } else
                CharacterModel_SetAnimationFlags(p + 0x610, 0x200);
        } else {
            CharacterModel_ClearAnimationFlags(p + 0x610, 0x200);
            CharacterModel_ClearAnimationFlags(p + 0x610, 1);
            ActorMarkerPool_ShowMarker(*((s8 *)(((u8 *)p) + 0x860)));
        }
        if ((!Inactive(p)) && (!(p && ((*((u16 *)(((u8 *)p) + 0x8F4))) & 4)))) {
            s32 amount;
            s32 *accumulator;
            s16 *resource;
            if (LinkMode(0x800))
                amount =
                    ModifierStats_ApplyKind11(view, ParticipantStat(*((s8 *)(((u8 *)p) + 0x8F8))));
            else
                amount = ModifierStats_ApplyKind11(view, *((s16 *)(((u8 *)gUnk_02000710) + 0x876)));
            accumulator = (s32 *)(p + 0x8FC);
            *accumulator += amount;
            resource = (s16 *)(view + 0x58);
            if ((*accumulator) > 6) {
                s32 delta = Div(*accumulator, 6);
                {
                    s32 v1 = (*resource) + delta;
                    *resource = v1;
                }
                *accumulator -= delta * 6;
                if ((*accumulator) < 0)
                    *accumulator = 0;
            }
            *resource += ModifierStats_ApplyKinds2And5(view, 0);
            if ((*resource) > (*((s16 *)(((u8 *)view) + 0x5A))))
                *resource = *((s16 *)(((u8 *)view) + 0x5A));
        }
    }
    *((u32 *)(((u8 *)p) + 0x8E4)) = *(*((u16 **)(p + 0x8C4)));
    if (!(p && ((*((u16 *)(((u8 *)p) + 0x8F4))) & 0x14)))
        (*((u32 *)(((u8 *)p) + 0x900)))++;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0811A158);
#endif
