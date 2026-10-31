#include "global.h"
#include "game_state.h"
#include "ram.h"
#include "engine_globals.h"
#include "area_effect_link.h"

union GloveAnimationKey {
    u32 word;
    u16 half[2];
};

struct GloveMotion {
    u8 pad0[4];
    u32 unk4;
    u32 unk8;
    u32 boundsFlags;
    u8 pad10[0x18 - 0x10];
    s32 z;
    u8 pad1C[4];
    s32 outX, outY, outZ; /* 0x20 */
    u8 pad2C[0x38 - 0x2C];
    s32 unk38;
    s32 unk3C;
    u32 flags40;
    u8 pad44[0x68 - 0x44];
    u32 pendingDamage;
    u8 pad6C[0x7E - 0x6C];
    u16 recoveryValue;
    u8 pad80[0xC0 - 0x80];
    s32 unkC0;
    u8 padC4[0x2B0 - 0xC4];
    u32 primaryCallback;
    u8 pad2B4, primaryKind;
    u8 pad2B6[0x305 - 0x2B6];
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
    u8 secondaryEnabled, pad325;
    u16 recoveryTimer, cooldown;
    u8 secondaryRelease, secondaryLatched;
    u8 pad32C[0x524 - 0x32C];
    union GloveAnimationKey animation;
    u8 pad528[0x57A - 0x528];
    u8 damageTimer;
    u8 pad57B[0x5AC - 0x57B];
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

struct GloveModel {
    u8 pad0[0x60];
    u8 primarySprite[0x180];
    u8 primaryAnimation[0x70];
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
    u8 pad8F9[0x914 - 0x8F9];
    s32 launchSpeed;
    u8 launchPhase;
};
s32 Div(s32, s32);
void Memory_Clear(void *, u32);
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
void Sound_StopSong(s32);
void Anim_SetKeyframes(void *, void *, u16, s32, s32);
void Anim_Step(void *, void *);
void PalFade_Start(void *, s32, u16, s32, s32, s32, s32, s32);
s32 CharacterModel_GetPrimaryPaletteIndex(void *);
s32 sub_08249240(void *, s32, u32);
void ChargeMarker_TickHold(void *, void *, u32);

static inline void Glove_BeginStopping(union GloveAnimationKey *animation, struct Glove *p,
                                       u8 *entry)
{
    u16 elapsedReset[] = {0};
    u8 entryClear = 0;
    animation->half[0] = 0x88;
    p->motion.animation.half[1] = elapsedReset[0];
    *entry = entryClear;
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
static inline void Glove_ClearDamageFlag(s8 *p, s32 mask)
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
                    Anim_SetKeyframes(p->model.primarySprite, p->model.primaryAnimation,
                                      p->motion.primaryKeyframes, 2, 0);
                    Anim_Step(p->model.primarySprite, p->model.primaryAnimation);
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
                Anim_SetKeyframes(p->model.primarySprite, p->model.primaryAnimation,
                                  p->motion.primaryKeyframes, 2, 0);
                Anim_Step(p->model.primarySprite, p->model.primaryAnimation);
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
            Anim_SetKeyframes(p->model.primarySprite, p->model.primaryAnimation,
                              p->motion.primaryKeyframes, 2, 0);
        Anim_Step(p->model.primarySprite, p->model.primaryAnimation);
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
void Glove_UpdateStopping(struct Glove *p)
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
                Glove_BeginStopping(&p->motion.animation, p, entry);
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
            self->outX = (h + self->x) - self->prevX;
            self->outY = self->y + self->dy;
            self->outZ = self->vz + self->dz;
            dx = (self->x - self->prevX) >> 12;
            dz = (self->z - self->prevZ) >> 12;
            lim = 0x3000;
            px = &self->x;
            pprevX = &self->prevX;
            py = &self->y;
            pprevZ = &self->prevZ;
            pvz = &self->vz;
            if (h > lim)
                dx = 0x80;
            if ((dx == 0) && (dz == 0))
                self->angle = 0xFFFF;
            else
                self->angle = (Math_Atan2(dx, dz) >> 5) << 5;
            *pprevX = *px;
            *pprevZ = self->z;
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
