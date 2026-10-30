#include "global.h"
#include "scene_actor_style.h"
#include "game_state.h"
#include "ram.h"
#include "engine_globals.h"
#include "area_effect_link.h"

// Clears action effects, emits defeat bursts, and selects the recovery callback.

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
    u8 pad10[0x18 - 0x10];
    s32 z;
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
    void *parts;
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
void Glove_SetAnimationKeyframes(struct Glove *, s32);
void ActorRecoil_UpdateMotion(struct Glove *);

void ActorParticles_UpdateEmitter(void *, void *, u32);
void GameState_AddActionCount894(s32);
void CharacterModel_ClearAnimationFlags(void *, u32);
void CharacterModel_SetAnimationFlags(void *, u32);
void ActorMarkerPool_ShowMarker(s32);
void Character_SetLowResourceColorEffect(void *, void *);
void Glove_UpdateStopping(struct Glove *);
void ActorParticles_SpawnSideBurst(struct GloveMotion *);
u8 LinkParticipant_IsLocallySelected(struct Glove *);
void Sound_StopSong(s32);
void Sound_PlaySong(s32);
void CallbackList_ResetDispatch(struct GloveMotion *);
void ActorMarker_HideSlot(s32);
void ActorMarkerPool_HideMarker(s32);
void Anim_SetKeyframes(void *, void *, u16, s32, s32);
void Anim_Step(void *, void *);
void SpritePair_ReleaseSlot(s32, s32);
void CharacterModel_LoadPrimaryPalette(void *);
void CharacterModel_LoadSecondaryPalette(void *, s32);
void sub_0810BB4C(void *, s32);
void sub_0810BB90(void *, s32);
s32 Mod(s32, s32);
void ActorParticles_EmitOffsetBurst(struct GloveMotion *);
void sub_08117F6C(struct Glove *);
void Glove_OnActionComplete(struct Glove *);
struct GloveModelSlotView {
    void **slot;
};
static inline void Glove_RestorePartPalettes(const struct GloveModelSlotView *view)
{
    void **sprites = view->slot;
    u32 paletteOffset = (u32)((struct GloveModel *)0)->palette;
    u32 offset = 0;
    s32 count = 4;
    do {
        struct GloveModel *base = *sprites;
        u8 *destination = (u8 *)&base->parts[0].palette;
        destination += offset;
        *(void **)destination = (u8 *)base + paletteOffset;
        offset += sizeof(struct GloveModelPart);
        count--;
    } while (count >= 0);
}
static inline void ClearFlags(u16 *p, s32 mask)
{
    *p &= mask;
}
static inline u8 IsRecoveryEnabled(u32 mask)
{
    if (gGameState->flags868 & mask)
        return 1;
    return 0;
}
void Glove_UpdateDefeatEffects(struct Glove *p)
{
    struct GloveMotion *self = &p->motion;
    s32 h;
    s32 dx, dz;
    u32 a, status;
    struct GloveMotion *actionSelf;
    s32 *bias;
    u32 *motionBias;
    u16 *flags;
    u8 *spriteSlot;
    u16 *actionTimer, *cooldown;
    u8 *actionPending, *releasePending, *style;
    void **sprites;
    s32 *vy, *py, *pvz;
    if (p->active) {
        struct GloveMotion *entrySelf;
        s32 count;
        u32 *animation;
        u32 hidden;
        u32 offset;
        u8 *actionKind;
        ActorParticles_SpawnSideBurst(self);
        if (LinkParticipant_IsLocallySelected(p)) {
            Sound_StopSong(0x87);
            Sound_StopSong(0x88);
            Sound_StopSong(0x89);
            Sound_StopSong(0x8A);
            Sound_StopSong(0x8B);
            Sound_StopSong(0x93);
        }
        entrySelf = self;
        actionKind = &p->motion.primaryKind;
        spriteSlot = &p->motion.secondarySpriteSlot;
        releasePending = &p->motion.secondaryLatched;
        actionPending = &p->motion.secondaryRelease;
        actionTimer = &p->motion.recoveryTimer;
        cooldown = &p->motion.cooldown;
        flags = &p->flags;
        sprites = &p->motion.parts;
        style = &p->motion.style.parameterD;
        motionBias = &p->motion.secondaryBias;
        hidden = 1;
        animation = &p->model.parts[0].flags;
        count = 4;
        do {
            *animation |= hidden;
            animation += sizeof(struct GloveModelPart) / sizeof(*animation);
            count--;
        } while (count >= 0);
        ActorMarkerPool_HideMarker(p->marker);
        p->active = 0;
        self->dx = 0;
        self->dz = 0;
        self->steps = 0;
        CallbackList_ResetDispatch(self);
        actionSelf = entrySelf;
        if (*actionKind == 2) {
            p->motion.primaryCharge = 0;
            ActorMarker_HideSlot(p->markerSlot);
        } else if (*actionKind == 5) {
            p->motion.primaryLatched = 1;
            Anim_SetKeyframes(&p->model.parts[1], p->model.primaryAnimation,
                              p->motion.primaryKeyframes, 2, 0);
            Anim_Step(&p->model.parts[1], p->model.primaryAnimation);
            p->motion.primarySpriteRefresh = 1;
            p->motion.primaryLatched = 0;
            if (LinkParticipant_IsLocallySelected(p))
                Sound_StopSong(0x93);
            actionSelf->actionActive = 0;
        } else if (*actionKind == 3) {
            Anim_SetKeyframes(&p->model.parts[1], p->model.primaryAnimation,
                              p->motion.primaryKeyframes, 2, 0);
            Anim_Step(&p->model.parts[1], p->model.primaryAnimation);
        }
        actionSelf->actionCooldown = 0;
        SpritePair_ReleaseSlot(*(s8 *)spriteSlot, 0);
        *releasePending = 0;
        *actionPending = 0;
        *actionTimer = 0;
        *cooldown = 0;
        ClearFlags(flags, ~2);
        {
            struct GloveModelSlotView view = {sprites};
            Glove_RestorePartPalettes(&view);
        }
        CharacterModel_LoadPrimaryPalette(&p->model);
        CharacterModel_LoadSecondaryPalette(&p->model, *style);
        {
            u32 mask = 0x800;
            if ((gEngineFlags | gScenePauseFlags) & mask) {
                struct AreaEffectLinkState *peer = gLinkSession;
                if (peer) {
                    if (peer->unavailable != 1)
                        peer = 0;
                }
                if (peer) {
                    void *data = peer->sessionData;
                    sub_0810BB4C(data, 1 - p->participant);
                    sub_0810BB90(data, p->participant);
                }
            }
        }
        self->vy = 0;
        self->vz = 0;
        self->unkC0 = 0;
        *motionBias = 0;
    }
    {
        u16 bit = 1;
        p->flags |= bit;
    }
    if (p->timer <= 29) {
        if (Mod(p->timer, 12) == 0) {
            ActorParticles_EmitOffsetBurst(self);
            Sound_PlaySong(0x16E);
        }
    } else if (p->timer <= 59) {
        if (Mod(p->timer, 6) == 0) {
            ActorParticles_EmitOffsetBurst(self);
            Sound_PlaySong(0x16E);
        }
    }
    {
        s32 *px, *idy, *idz;
        s32 decay;
        h = self->vy + self->unkC0;
        {
            s32 limit = 0xF000;
            bias = &self->unkC0;
            vy = &self->vy;
            if (h > limit)
                h = limit;
        }

        self->y += 0x200;
        decay = self->steps;
        py = &self->y;
        if (decay > 0) {
            self->x += self->dx;
            self->dx += Div(-self->dx, self->steps);
            self->dy += Div(-self->dy, self->steps);
            self->dz += Div(-self->dz, self->steps);
            self->steps--;
            px = &self->x;
            idy = &self->dy;
            idz = &self->dz;
        } else {
            Memory_Clear(&self->dx, 0x10);
            px = &self->x;
            idy = &self->dy;
            idz = &self->dz;
        }
        Actor_ClampMovementSpeed(self);

        {
            s32 *pprevZ, *pprevX;
            s32 lim;
            self->outX = *px + h - self->prevX;
            self->outY = *py + *idy;
            self->outZ = self->vz + *idz;
            dx = (*px - self->prevX) >> 12;
            dz = (self->z - self->prevZ) >> 12;
            lim = 0x3000;
            pprevX = &self->prevX;
            pvz = &self->vz;
            pprevZ = &self->prevZ;
            if (h > lim)
                dx = 0x80;
            if (dx == 0 && dz == 0)
                self->angle = 0xFFFF;
            else
                self->angle = (Math_Atan2(dx, dz) >> 5) << 5;
            *pprevX = *px;
            *pprevZ = self->z;
            Terrain_ResolveActorMotion(self);
            a = (u32) ~(self->unk3C >> 12) >> 31;
            status = (u32) ~(self->unk38 >> 12) >> 31;
            if (self->unk4 != self->unk8 && (self->unk4 & 0x1000) && !(self->unk8 & 0xF000) &&
                status <= 1 && a == 1) {
                s32 v = ((*vy >> 10) + (*bias >> 8)) >> 1;
                if (v < 4)
                    v = 4;
                if (v > 18)
                    v = 18;
                *py -= v << 9;
                self->flags40 &= ~1;
            }
            p->height = h;

            Actor_UpdateShadowSpriteParts(self, &p->model);
            if (self->flags40 & 2)
                *pvz = 0;
            if (self->flags40 & 1)
                *py = 0;
        }
    }
    if (p->timer > 120) {
        void (*callback)(void *);
        u8 *active;
        u32 mask = 0x800;
        if (((gEngineFlags | gScenePauseFlags) & mask) || (status = IsRecoveryEnabled(0x100))) {
            callback = (void (*)(void *))sub_08117F6C;
            active = &p->active;
            status = 0;
        } else {
            callback = (void (*)(void *))Glove_OnActionComplete;
            active = &p->active;
        }
        *active = 1;
        p->update = callback;
        p->timer = status;
    } else
        p->timer++;
}
