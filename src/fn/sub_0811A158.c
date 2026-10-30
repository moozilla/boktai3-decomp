#include "global.h"
#include "scene_actor_style.h"
#include "game_state.h"
#include "ram.h"
#include "engine_globals.h"
#include "area_effect_link.h"

struct GloveModel;
union GloveAnimationKey {
    u32 word;
    u16 half[2];
};

struct GloveMotion {
    u8 pad0[4];
    u32 unk4;
    u32 unk8;
    u32 boundsFlags;
    s32 positionX, positionY, z;
    u8 pad1C[4];
    s32 outX, outY, outZ; /* 0x20 */
    u8 pad2C[0x38 - 0x2C];
    s32 unk38;
    s32 unk3C;
    u32 flags40;
    u8 color[4];
    struct SceneActorStyle style;
    u8 pad64[4];
    u32 pendingDamage;
    u8 pad6C[0x7E - 0x6C];
    u16 recoveryValue;
    struct GloveModel *parts;
    u8 pad84[0xC0 - 0x84];
    s32 unkC0;
    u8 padC4[4];
    u32 secondaryBias;
    u8 padCC[0x2B0 - 0xCC];
    u32 primaryCallback;
    u8 pad2B4, primaryKind;
    u8 pad2B6[8];
    u16 primaryCharge;
    u8 pad2C0[0x305 - 0x2C0];
    u8 primarySpriteRefresh, primaryLatched;
    u8 actionActive;
    u16 actionCooldown;
    u8 pad30A[4];
    u16 primaryKeyframes;
    u8 pad310[4];
    u32 secondaryCallback;
    u8 pad318;
    u8 actionKind;
    u8 pad31A[0x324 - 0x31A];
    u8 secondaryEnabled;
    s8 secondarySpriteSlot;
    u16 recoveryTimer, cooldown;
    u8 secondaryRelease, secondaryLatched;
    u8 pad32C[0x524 - 0x32C];
    union GloveAnimationKey animation;
    u8 pad528[4];
    u8 contactIndex[0x40];
    u32 motionFlags;
    u8 pad570[10];
    u8 damageTimer;
    u8 pad57B[0x59E - 0x57B];
    u16 spatialTimer;
    u8 pad5A0[0x5AC - 0x5A0];
    s32 vy; /* 0x5AC */
    s32 y;  /* 0x5B0 */
    s32 vz; /* 0x5B4 */
    u8 pad5B8[4];
    s32 prevX; /* 0x5BC */
    u8 pad5C0[4];
    s32 prevZ; /* 0x5C4 */
    u8 pad5C8[4];
    s32 dx; /* 0x5CC */
    s32 dy; /* 0x5D0 */
    s32 dz; /* 0x5D4 */
    u8 pad5D8[4];
    s32 x; /* 0x5DC */
    u8 pad5E0[0x5EC - 0x5E0];
    s8 steps; /* 0x5EC */
    u8 pad5ED;
    s8 damageFlags;
    u8 pad5EF;
    u16 angle; /* 0x5F0 */
    u8 pad5F2[6];
};

struct GloveModelPart {
    u8 pad0[8];
    u32 flags;
    u8 padC[0x48 - 0xC];
    void *palette;
    u8 pad4C[0x60 - 0x4C];
};
struct GloveModel {
    struct GloveModelPart parts[5];
    u8 primaryAnimation[0x30];
    u8 palette[0x40];
};

struct Glove {
    u8 pad0[0x18];
    struct GloveMotion motion;
    struct GloveModel model;
    s8 marker, markerSlot;
    u8 pad862[2];
    u32 timer;
    u8 active;
    u8 pad869[3];
    void (*update)(void *);
    u8 pad870[0x8C4 - 0x870];
    u16 *input;
    u8 pad8C8[0x8E4 - 0x8C8];
    u32 previousInput;
    s32 height;
    u8 pad8EC[8];
    u16 flags, blinkTimer;
    s8 participant;
    u8 pad8F9[3];
    s32 resourceFraction;
    u32 activeFrames;
    u8 pad904[0x914 - 0x904];
    s32 launchSpeed;
    u8 launchPhase;
};
struct Vec16 {
    s16 x, y, z;
};
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
    return (gEngineFlags | gScenePauseFlags) & mask;
}
static inline u32 Glove_ReadFlags(struct Glove *p)
{
    return p->flags;
}
static inline u32 Glove_ActorFlags(struct Glove *p, u32 mask)
{
    if (!p)
        return 0;
    return Glove_ReadFlags(p) & mask;
}
static inline u8 Inactive(struct Glove *p)
{
    if (p) {
        if (Glove_ReadFlags(p) & 1)
            return 1;
    }
    return 0;
}
static inline s32 LocalParticipant(void)
{
    struct AreaEffectLinkState *p = gLinkSession;
    if (!p)
        return -1;
    return p->localPlayer;
}
static inline s16 Participant_Index(s32 slot)
{
    return slot;
}
static inline u8 ParticipantStatIndexed(s32 slot)
{
    s32 index = Participant_Index(slot);
    struct AreaEffectLinkState *p = gLinkSession;
    if (!p || slot < 0)
        return 0;
    return p->playerValues[index];
}
static inline void Glove_UpdateLocalPresentation(struct Glove *p)
{
    struct GloveMotion *view = &p->motion;
    if (LinkMode(0x800)) {
        if ((p->participant) == LocalParticipant()) {
            gUnk_020004E0[0] = ((view->positionX) - (view->x)) >> 12;
            gUnk_020004E0[2] = (view->z) >> 12;
            if (p && ((Glove_ReadFlags(p)) & 4))
                PaletteFade_SetScrollPosition(0, 0);
            else
                PaletteFade_SetScrollPosition((p->height) >> 12, 0);
        }
    } else {
        gUnk_020004E0[0] = ((view->positionX) - (p->motion.x)) >> 12;
        gUnk_020004E0[2] = (view->z) >> 12;
        if (p && ((Glove_ReadFlags(p)) & 4))
            PaletteFade_SetScrollPosition(0, 0);
        else
            PaletteFade_SetScrollPosition((p->height) >> 12, 0);
    }
}
static inline s32 *Glove_AccumulateResource(struct Glove *p, s32 amount)
{
    s32 *balance = &p->resourceFraction;
    *balance += amount;
    return balance;
}
static inline void Glove_ClearAnimationFlags(struct GloveModel *model, u32 flags)
{
    CharacterModel_ClearAnimationFlags(model, flags);
}
static inline s16 Glove_SignedHeading(u16 raw)
{
    return (s16)raw;
}
// confidence=low: updates spatial entries, local presentation and resource recovery.
void Glove_UpdatePostState(struct Glove *p)
{
    struct GloveMotion *self = &p->motion;
    if (!Inactive(p)) {
        struct Vec16 pos;
        u32 *motion;
        s32 heading;
        Character_GetHeightAdjustedPosition(self, &pos);
        SpatialIndex_SetPosition(&self->motionFlags, pos.x, pos.y, pos.z);
        heading = Glove_SignedHeading(self->angle);
        motion = &self->motionFlags;
        if (heading < 0) {
            SpatialIndex_SetPosition(self->contactIndex, pos.x, pos.y, pos.z);
        } else {
            struct Vec16 off;
            struct Vec16 *o = &off;
            s32 angle = (s16)self->angle;
            s32 radius = 20;
            o->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
            o->y = 0;
            o->z = Scale(gUnk_086149C4[angle & 255] * radius);
            SpatialIndex_SetPosition(self->contactIndex, pos.x + off.x, pos.y, pos.z + off.z);
        }
        if (self->spatialTimer) {
            (self->spatialTimer)--;
            *motion |= 1;
        } else
            *motion &= ~1;
        SpatialIndex_InsertEntry(self->contactIndex);
    }
    {
        Glove_UpdateLocalPresentation(p);
        if (!(p && ((Glove_ReadFlags(p)) & 0x14)))
            ActorStats_ApplyPendingDamage(self);
        Character_UpdateTimedEffect(&self->pendingDamage);
        if ((!Inactive(p)) && (!(p && ((Glove_ReadFlags(p)) & 0x10))))
            ActorParticles_UpdateEmitter(self, self->color, self->style.initialWord);
        if (Glove_ActorFlags(p, 1)) {
            p->resourceFraction = 0;
            p->motion.recoveryTimer = 0;
        } else if (Glove_ActorFlags(p, 2)) {
            if (LinkMode(0x800)) {
                if ((p->participant) == LocalParticipant()) {
                    CharacterModel_SetAnimationFlags(&p->model, 0x200);
                } else {
                    CharacterModel_SetAnimationFlags(&p->model, 1);
                    ActorMarkerPool_HideMarker(p->marker);
                }
            } else
                CharacterModel_SetAnimationFlags(&p->model, 0x200);
        } else {
            Glove_ClearAnimationFlags(&p->model, 0x200);
            CharacterModel_ClearAnimationFlags(&p->model, 1);
            ActorMarkerPool_ShowMarker(p->marker);
        }
        {
            struct GloveMotion *view = &p->motion;
            if ((!Inactive(p)) && (!(p && ((Glove_ReadFlags(p)) & 4)))) {
                s32 *accumulator;
                s16 *resource;
                s32 balance;
                if (LinkMode(0x800)) {
                    s32 participant = p->participant;
                    accumulator = Glove_AccumulateResource(
                        p, ModifierStats_ApplyKind11(view, ParticipantStatIndexed(participant)));
                } else
                    accumulator = Glove_AccumulateResource(
                        p, ModifierStats_ApplyKind11(view, gGameState->unk876));
                balance = *accumulator;
                resource = &view->style.initialHalfword;
                if (balance > 6) {
                    s32 delta = Div(*accumulator, 6);
                    {
                        s32 v1 = (u16)*resource + delta;
                        *resource = v1;
                    }
                    *accumulator -= delta * 6;
                    if ((*accumulator) < 0)
                        *accumulator = 0;
                }
                *resource += ModifierStats_ApplyKinds2And5(view, 0);
                if ((*resource) > (view->style.currentHalfword))
                    *resource = view->style.currentHalfword;
            }
        }
    }
    p->previousInput = *p->input;
    if (!(p && ((Glove_ReadFlags(p)) & 0x14)))
        (p->activeFrames)++;
}
