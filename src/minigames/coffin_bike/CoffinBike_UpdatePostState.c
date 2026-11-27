#include "global.h"
#include "scene/actor/scene_actor_style.h"
#include "system/game_state/game_state.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
#include "scene/areas/area_effect_link.h"
#include "engine/engine_pause.h"
#include "bosses/vanargand/vanargand_projectiles.h"
#include "minigames/coffin_bike/coffin_bike.h"

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
void Character_UpdateTimedEffect(struct CharacterPreviewPalette *);
void ActorParticles_UpdateEmitter(void *, void *, u32);
void CharacterModel_SetAnimationFlags(void *, u32);
void CharacterModel_ClearAnimationFlags(void *, u32);
void ActorMarkerPool_HideMarker(s32);
void ActorMarkerPool_ShowMarker(s32);
s32 ModifierStats_ApplyKind11(void *, s32);
s32 ModifierStats_ApplyKinds2And5(void *, s32);
s32 Div(s32, s32);
static inline u32 CoffinBike_ReadFlags(struct CoffinBike *p)
{
    return p->flags;
}
static inline u32 CoffinBike_ActorFlags(struct CoffinBike *p, u32 mask)
{
    if (!p)
        return 0;
    return CoffinBike_ReadFlags(p) & mask;
}
static inline u8 Inactive(struct CoffinBike *p)
{
    if (p) {
        if (CoffinBike_ReadFlags(p) & 1)
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
static inline void CoffinBike_UpdateLocalPresentation(struct CoffinBike *p)
{
    struct CoffinBikeMotion *view = &p->motion;
    if (Engine_PauseBits(0x800)) {
        if ((p->participant) == LocalParticipant()) {
            gUnk_020004E0[0] = ((view->position.x) - (view->x)) >> 12;
            gUnk_020004E0[2] = (view->position.z) >> 12;
            if (p && ((CoffinBike_ReadFlags(p)) & 4))
                PaletteFade_SetScrollPosition(0, 0);
            else
                PaletteFade_SetScrollPosition((p->height) >> 12, 0);
        }
    } else {
        gUnk_020004E0[0] = ((view->position.x) - (p->motion.x)) >> 12;
        gUnk_020004E0[2] = (view->position.z) >> 12;
        if (p && ((CoffinBike_ReadFlags(p)) & 4))
            PaletteFade_SetScrollPosition(0, 0);
        else
            PaletteFade_SetScrollPosition((p->height) >> 12, 0);
    }
}
static inline s32 *CoffinBike_AccumulateResource(struct CoffinBike *p, s32 amount)
{
    s32 *balance = &p->resourceFraction;
    *balance += amount;
    return balance;
}
static inline void CoffinBike_ClearAnimationFlags(struct CharacterPreviewModel *model, u32 flags)
{
    CharacterModel_ClearAnimationFlags(model, flags);
}
static inline s16 CoffinBike_SignedHeading(u16 raw)
{
    return (s16)raw;
}
// confidence=low: updates spatial entries, local presentation and resource recovery.
void CoffinBike_UpdatePostState(struct CoffinBike *p)
{
    struct CoffinBikeMotion *self = &p->motion;
    if (!Inactive(p)) {
        struct Vec16 pos;
        u32 *motion;
        s32 heading;
        Character_GetHeightAdjustedPosition(self, &pos);
        SpatialIndex_SetPosition(&self->motionFlags, pos.x, pos.y, pos.z);
        heading = CoffinBike_SignedHeading(self->angle);
        motion = &self->motionFlags;
        if (heading < 0) {
            SpatialIndex_SetPosition(&self->contact, pos.x, pos.y, pos.z);
        } else {
            struct Vec16 off;
            struct Vec16 *o = &off;
            s32 angle = (s16)self->angle;
            s32 radius = 20;
            o->x = VanargandProjectile_Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
            o->y = 0;
            o->z = VanargandProjectile_Scale(gUnk_086149C4[angle & 255] * radius);
            SpatialIndex_SetPosition(&self->contact, pos.x + off.x, pos.y, pos.z + off.z);
        }
        if (self->spatialTimer) {
            (self->spatialTimer)--;
            *motion |= 1;
        } else
            *motion &= ~1;
        SpatialIndex_InsertEntry(&self->contact);
    }
    {
        CoffinBike_UpdateLocalPresentation(p);
        if (!(p && ((CoffinBike_ReadFlags(p)) & 0x14)))
            ActorStats_ApplyPendingDamage(self);
        Character_UpdateTimedEffect(&self->palette);
        if ((!Inactive(p)) && (!(p && ((CoffinBike_ReadFlags(p)) & 0x10))))
            ActorParticles_UpdateEmitter(self, &self->emitter, self->style.initialWord);
        if (CoffinBike_ActorFlags(p, 1)) {
            p->resourceFraction = 0;
            p->motion.recoveryTimer = 0;
        } else if (CoffinBike_ActorFlags(p, 2)) {
            if (Engine_PauseBits(0x800)) {
                if ((p->participant) == LocalParticipant()) {
                    CharacterModel_SetAnimationFlags(&p->model, 0x200);
                } else {
                    CharacterModel_SetAnimationFlags(&p->model, 1);
                    ActorMarkerPool_HideMarker((s8)p->model.primaryAppearance);
                }
            } else
                CharacterModel_SetAnimationFlags(&p->model, 0x200);
        } else {
            CoffinBike_ClearAnimationFlags(&p->model, 0x200);
            CharacterModel_ClearAnimationFlags(&p->model, 1);
            ActorMarkerPool_ShowMarker((s8)p->model.primaryAppearance);
        }
        {
            struct CoffinBikeMotion *view = &p->motion;
            if ((!Inactive(p)) && (!(p && ((CoffinBike_ReadFlags(p)) & 4)))) {
                s32 *accumulator;
                s16 *resource;
                s32 balance;
                if (Engine_PauseBits(0x800)) {
                    s32 participant = p->participant;
                    accumulator = CoffinBike_AccumulateResource(
                        p, ModifierStats_ApplyKind11(view, ParticipantStatIndexed(participant)));
                } else
                    accumulator = CoffinBike_AccumulateResource(
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
    if (!(p && ((CoffinBike_ReadFlags(p)) & 0x14)))
        (p->activeFrames)++;
}
