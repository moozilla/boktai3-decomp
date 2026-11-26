#include "global.h"
#include "system/game_state/game_state.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
#include "scene/areas/area_effect_link.h"
#include "minigames/coffin_bike/coffin_bike.h"

s32 Div(s32, s32);
s32 Math_Atan2(s32, s32);
void Terrain_ResolveActorMotion(struct GloveMotion *);
void PalFade_Update(struct GloveMotion *);
void Actor_UpdateShadowSpriteParts(struct GloveMotion *, void *);
void ActorParticles_SpawnImpactBurst(struct GloveMotion *);
void CharacterModel_UpdateAccessoryAnimations(void *);
void ScenePool_ClampMotionSpeeds(struct GloveMotion *);
void Actor_ClampMovementSpeed(struct GloveMotion *);
void MotionActor_UpdateMovementAudio(struct Glove *);
u8 LinkParticipant_IsLocallySelected(struct Glove *);
void PalFade_Start(void *, s32, u16, s32, s32, s32, s32, s32);
s32 CharacterModel_GetPrimaryPaletteIndex(void *);
void ChargeMarker_TickHold(void *, void *, u32);

static inline void CoffinBike_BeginStopping(union GloveAnimationKey *animation, struct Glove *p,
                                       u8 *entry)
{
    u16 elapsedReset[] = {0};
    animation->half[0] = 0x88;
    p->motion.animation.half[1] = elapsedReset[0];
    *entry = 0;
}
static inline s32 Glove_ReadHeightLimit(const s32 *limit)
{
    return *limit;
}
struct GloveFlagBinding {
    u16 *field;
};
static inline u16 *Glove_FlagsFromBinding(const struct GloveFlagBinding *binding)
{
    return binding->field;
}
struct GloveMotionBinding {
    struct GloveMotion *field;
};
static inline struct GloveMotion *Glove_MotionFromBinding(struct GloveMotionBinding binding)
{
    return binding.field;
}
static inline void Glove_ClearDamageFlag(u8 *p, s32 mask)
{
    *p &= mask;
}
static inline void Glove_UpdateSecondaryAction(struct Glove *p, struct GloveMotion *self,
                                               u16 *flags)
{
    s32 palette;
    if (p->motion.secondaryEnabled) {
        if ((p->motion.actionKind) == 5) {
            palette = CharacterModel_GetPrimaryPaletteIndex(&p->model);
            if (p->input[0] & 1) {
                if (!sub_08249240(&p->motion.secondaryCallback, 0, p->motion.secondaryCallback)) {
                    if ((p->motion.secondaryLatched) == 1) {
                        PalFade_Start(self, palette, p->motion.recoveryValue, 0, 0, 16, 1, 1);
                        p->motion.secondaryLatched = 0;
                    }
                    p->motion.secondaryRelease = 0;
                }
            } else if ((p->motion.secondaryLatched) && (p->previousInput & 1)) {
                PalFade_Start(self, palette, p->motion.recoveryValue, 0, 0, 16, 1, 1);
                p->motion.secondaryRelease = 0;
            }
        } else if (p->input[1] & 1)
            sub_08249240(&p->motion.secondaryCallback, 0, p->motion.secondaryCallback);
        if (self->actionKind == 4) {
            if (self->recoveryTimer) {
                u16 bit = 2;
                *flags |= bit;
                self->recoveryTimer--;
            }
        } else if ((self->actionKind == 1) && self->recoveryTimer) {
            self->recoveryTimer--;
            Glove_ClearDamageFlag(&self->damageFlags, ~1);
            self->damageTimer = 0;
            self->pendingDamage = 0;
            if (!self->recoveryTimer) {
                palette = CharacterModel_GetPrimaryPaletteIndex(&p->model);
                PalFade_Start(self, palette, self->recoveryValue, 0, 0, 16, 1, 1);
            }
        }
        if (self->cooldown)
            self->cooldown--;
    }
}

static inline void Glove_UpdatePrimaryAction(struct Glove *p, struct GloveMotion *self, u32 kind)
{
    if (kind == 2) {
        ChargeMarker_TickHold(self, &p->model, (u32)p->input);
    } else if (kind == 5) {
        if (p->input[0] & 2) {
            if (!sub_08249240(&p->motion.primaryCallback, 0, p->motion.primaryCallback)) {
                if (p->motion.primaryLatched) {
                    Anim_SetKeyframes(&p->model.parts[1], p->model.primaryAnimation,
                                      p->motion.primaryKeyframes, 2, 0);
                    Anim_Step(&p->model.parts[1], p->model.primaryAnimation);
                    p->motion.primarySpriteRefresh = 1;
                    p->motion.primaryLatched = 0;
                    if (LinkParticipant_IsLocallySelected(p))
                        Sound_StopSong(0x93);
                }
                self->actionActive = 1;
            } else {
                p->motion.primaryLatched = 1;
            }
        } else {
            if (p->motion.primaryLatched) {
                Anim_SetKeyframes(&p->model.parts[1], p->model.primaryAnimation,
                                  p->motion.primaryKeyframes, 2, 0);
                Anim_Step(&p->model.parts[1], p->model.primaryAnimation);
                p->motion.primarySpriteRefresh = 1;
                p->motion.primaryLatched = 0;
                if (LinkParticipant_IsLocallySelected(p))
                    Sound_StopSong(0x93);
            }
            self->actionActive = 0;
        }
    } else if (kind == 3) {
        if (p->input[1] & 2)
            sub_08249240(&p->motion.primaryCallback, 0, p->motion.primaryCallback);
        if (p->motion.actionCooldown == 1)
            Anim_SetKeyframes(&p->model.parts[1], p->model.primaryAnimation,
                              p->motion.primaryKeyframes, 2, 0);
        Anim_Step(&p->model.parts[1], p->model.primaryAnimation);
    } else {
        if (p->input[1] & 2)
            sub_08249240(&p->motion.primaryCallback, 0, p->motion.primaryCallback);
    }
    if (self->actionCooldown)
        self->actionCooldown--;
}
static inline struct GloveMotion *Glove_BorrowMotion(const struct GloveMotionBinding *binding)
{
    return binding->field;
}
static inline u16 Glove_DamageFlagWord(const u16 *field)
{
    return *field;
}
static inline struct GloveModel *Glove_ModelFromWorkOwner(void *const *object)
{
    return &((struct Glove *)*object)->model;
}
static inline void Glove_UpdateDamageAndAccessories(struct Glove *p,
                                                    struct GloveMotionBinding binding, u16 *flags)
{
    void *object = Glove_BorrowMotion(&binding);
    if ((!p || !(Glove_DamageFlagWord(flags) & 4)) &&
        (((struct GloveMotion *)object)->boundsFlags & 0x800)) {
        u8 bit = 4;
        ((struct GloveMotion *)object)->damageFlags |= bit;
    }
    object = p;
    CharacterModel_UpdateAccessoryAnimations(Glove_ModelFromWorkOwner(&object));
}
void CoffinBike_UpdateStopping(struct Glove *p)
{
    struct GloveMotion *self = &p->motion;
    s32 h;
    s32 dx;
    s32 dz;
    u32 a;
    u32 b;
    struct GloveMotion *actionMotion;
    struct GloveMotionBinding secondaryBinding;
    u32 primaryKind;
    {
        u8 *entry = &p->active;
        if (*entry) {
            {
                CoffinBike_BeginStopping(&p->motion.animation, p, entry);
            }
        }
    }
    {
        s32 heightLimit;
        u16 *flags;
        struct GloveFlagBinding movementFlags;
        movementFlags.field = &p->flags;
        MotionActor_UpdateMovementAudio(p);
        ScenePool_ClampMotionSpeeds(self);
        h = self->vy + self->unkC0;
        heightLimit = 0xF000;
        flags = Glove_FlagsFromBinding(&movementFlags);
        if (h > Glove_ReadHeightLimit(&heightLimit))
            h = Glove_ReadHeightLimit(&heightLimit);
        self->y += 0x200;
        if (self->steps > 0) {
            self->x += self->dx;
            self->dx += Div(-self->dx, self->steps);
            self->dy += Div(-self->dy, self->steps);
            self->dz += Div(-self->dz, self->steps);
            self->steps--;
        } else {
            Memory_Clear(&self->dx, 0x10);
        }
        Actor_ClampMovementSpeed(self);
        {
            s32 *pprevZ;
            s32 *pprevX;
            s32 *pvz;
            s32 *px;
            s32 *py;
            s32 lim;
            self->outX = (h + self->x) - self->previousPosition.x;
            self->outY = self->y + self->dy;
            self->outZ = self->vz + self->dz;
            dx = (self->x - self->previousPosition.x) >> 12;
            dz = (self->position.z - self->previousPosition.z) >> 12;
            lim = 0x3000;
            px = &self->x;
            pprevX = &self->previousPosition.x;
            py = &self->y;
            pprevZ = &self->previousPosition.z;
            pvz = &self->vz;
            if (h > lim)
                dx = 0x80;
            if ((dx == 0) && (dz == 0))
                self->angle = 0xFFFF;
            else
                self->angle = (Math_Atan2(dx, dz) >> 5) << 5;
            *pprevX = *px;
            *pprevZ = self->position.z;
            Terrain_ResolveActorMotion(self);
            a = ((u32)(~(self->unk3C >> 12))) >> 31;
            b = ((u32)(~(self->unk38 >> 12))) >> 31;
            if (((((self->unk4 != self->unk8) && (self->unk4 & 0x1000)) &&
                  (!(self->unk8 & 0xF000))) &&
                 (b <= 1)) &&
                (a == 1)) {
                s32 v = ((self->vy >> 10) + (self->unkC0 >> 8)) >> 1;
                if (v < 4)
                    v = 4;
                if (v > 18)
                    v = 18;
                *py -= v << 9;
                self->flags40 &= ~1;
            }
            p->height = h;
            PalFade_Update(self);
            Actor_UpdateShadowSpriteParts(self, &p->model);
            if (self->flags40 & 4)
                ActorParticles_SpawnImpactBurst(self);
            if (self->flags40 & 2)
                *pvz = 0;
            if (self->flags40 & 1)
                *py = 0;
        }
        actionMotion = &p->motion;
        primaryKind = p->motion.primaryKind;
        secondaryBinding.field = actionMotion;
        Glove_UpdatePrimaryAction(p, actionMotion, primaryKind);
        Glove_UpdateSecondaryAction(p, Glove_MotionFromBinding(secondaryBinding), flags);
        Glove_UpdateDamageAndAccessories(p, secondaryBinding, flags);
    }
    p->timer++;
}
