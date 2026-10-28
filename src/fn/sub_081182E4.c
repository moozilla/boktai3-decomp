#include "global.h"
#include "game_state.h"
#include "ram.h"
#include "engine_globals.h"
#include "area_effect_link.h"

// confidence=low: clears actions and markers, emits defeat bursts, then selects recovery.

// MATCHING_CONCESSION: register-pinning
#ifdef NONMATCHING
// fuzzy: 86.6%
// Reset-action pointer lifetimes, motion temporaries and register allocation still differ.

struct Obj {
    u8 pad0[4];
    u32 unk4;
    u32 unk8;
    u8 padC[0x18 - 0xC];
    s32 z;
    u8 pad1C[4];
    s32 outX, outY, outZ; /* 0x20 */
    u8 pad2C[0x38 - 0x2C];
    s32 unk38;
    s32 unk3C;
    u32 flags40;
    u8 pad44[0xC0 - 0x44];
    s32 unkC0;
    u8 padC4[0x5AC - 0xC4];
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
    u8 pad5ED[3];
    u16 angle; /* 0x5F0 */
    u8 pad5F2[6];
};

struct Glove {
    u8 pad0[0x18];
    struct Obj motion;
    u8 model[0x860 - 0x610];
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
#define U8(p, o)  (*(u8 *)((u8 *)(p) + (o)))
#define S8(p, o)  (*(s8 *)((u8 *)(p) + (o)))
#define U16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S16(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define U32(p, o) (*(u32 *)((u8 *)(p) + (o)))
#define S32(p, o) (*(s32 *)((u8 *)(p) + (o)))
s32 Div(s32, s32);
void Memory_Clear(void *, u32);
s32 Math_Atan2(s32, s32);
void Terrain_ResolveActorMotion(struct Obj *);
void PalFade_Update(struct Obj *);
void Actor_UpdateShadowSpriteParts(struct Obj *, void *);
void ActorParticles_SpawnImpactBurst(struct Obj *);
void CharacterModel_UpdateAccessoryAnimations(void *);
void ScenePool_ClampMotionSpeeds(struct Obj *);
void Actor_ClampMovementSpeed(struct Obj *);
void Glove_SetAnimationKeyframes(struct Glove *, s32);
void ActorRecoil_UpdateMotion(struct Glove *);

void ActorParticles_UpdateEmitter(void *, void *, u32);
void GameState_AddActionCount894(s32);
void CharacterModel_ClearAnimationFlags(void *, u32);
void CharacterModel_SetAnimationFlags(void *, u32);
void ActorMarkerPool_ShowMarker(s32);
void Character_SetLowResourceColorEffect(void *, void *);
void Glove_UpdateStopping(struct Glove *);
void ActorParticles_SpawnSideBurst(struct Obj *);
u8 LinkParticipant_IsLocallySelected(struct Glove *);
void Sound_StopSong(s32);
void Sound_PlaySong(s32);
void CallbackList_ResetDispatch(struct Obj *);
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
void ActorParticles_EmitOffsetBurst(struct Obj *);
void sub_08117F6C(struct Glove *);
void Glove_OnActionComplete(struct Glove *);
static inline void ClearFlags(u16 *p, s32 mask)
{
    *p &= mask;
}
static inline u8 IsRecoveryEnabled(u32 mask)
{
    if (U32(gGameState, 0x868) & mask)
        return 1;
    return 0;
}
void Glove_UpdateDefeatEffects(struct Glove *p)
{
    struct Obj *self = &p->motion;
    register s32 h asm("sl");
    s32 dx, dz;
    u32 a, b;
    if (p->active) {
        s32 count;
        u8 *animation;
        u32 offset;
        struct Obj *actionSelf;
        u8 *actionKind, *spriteSlot, *releasePending, *actionPending;
        u16 *actionTimer, *cooldown, *flags;
        u8 **sprites;
        u32 *motionBias;
        u8 *style;
        ActorParticles_SpawnSideBurst(self);
        if (LinkParticipant_IsLocallySelected(p)) {
            Sound_StopSong(0x87);
            Sound_StopSong(0x88);
            Sound_StopSong(0x89);
            Sound_StopSong(0x8A);
            Sound_StopSong(0x8B);
            Sound_StopSong(0x93);
        }
        actionSelf = self;
        actionKind = (u8 *)p + 0x2CD;
        spriteSlot = (u8 *)p + 0x33D;
        releasePending = (u8 *)p + 0x343;
        actionPending = (u8 *)p + 0x342;
        actionTimer = (u16 *)((u8 *)p + 0x33E);
        cooldown = (u16 *)((u8 *)p + 0x340);
        flags = &p->flags;
        sprites = (u8 **)((u8 *)p + 0x98);
        style = (u8 *)p + 0x63;
        motionBias = (u32 *)((u8 *)p + 0xE0);
        animation = p->model + 8;
        count = 4;
        do {
            U32(animation, 0) |= 1;
            animation += 0x60;
            count--;
        } while (count >= 0);
        ActorMarkerPool_HideMarker(p->marker);
        p->active = 0;
        self->dx = 0;
        self->dz = 0;
        self->steps = 0;
        CallbackList_ResetDispatch(self);
        if (*actionKind == 2) {
            U16(p, 0x2D6) = 0;
            ActorMarker_HideSlot(p->markerSlot);
        } else if (*actionKind == 5) {
            U8(p, 0x31E) = 1;
            Anim_SetKeyframes((u8 *)p + 0x670, (u8 *)p + 0x7F0, U16(p, 0x326), 2, 0);
            Anim_Step((u8 *)p + 0x670, (u8 *)p + 0x7F0);
            U8(p, 0x31D) = 1;
            U8(p, 0x31E) = 0;
            if (LinkParticipant_IsLocallySelected(p))
                Sound_StopSong(0x93);
            U8(actionSelf, 0x307) = 0;
        } else if (*actionKind == 3) {
            Anim_SetKeyframes((u8 *)p + 0x670, (u8 *)p + 0x7F0, U16(p, 0x326), 2, 0);
            Anim_Step((u8 *)p + 0x670, (u8 *)p + 0x7F0);
        }
        U16(actionSelf, 0x308) = 0;
        SpritePair_ReleaseSlot(*(s8 *)spriteSlot, 0);
        *releasePending = 0;
        *actionPending = 0;
        *actionTimer = 0;
        *cooldown = 0;
        ClearFlags(flags, ~2);
        offset = 0;
        count = 4;
        do {
            u8 *base = *sprites;
            u8 *destination = base + 0x48;
            destination += offset;
            *(u32 *)destination = (u32)(base + 0x210);
            offset += 0x60;
            count--;
        } while (count >= 0);
        CharacterModel_LoadPrimaryPalette(p->model);
        CharacterModel_LoadSecondaryPalette(p->model, *style);
        {
            u32 mask = 0x800;
            if ((gEngineFlags | gScenePauseFlags) & mask) {
                u8 *peer = gLinkSession;
                if (peer) {
                    if (U8(peer, 0x53) != 1)
                        peer = 0;
                }
                if (peer) {
                    peer += 0xE0;
                    sub_0810BB4C(peer, 1 - p->participant);
                    sub_0810BB90(peer, p->participant);
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
        s32 *vy = &self->vy, *bias = &self->unkC0;
        s32 *py, *px, *idy, *idz;
        h = *vy + *bias;
        if (h > 0xF000)
            h = 0xF000;

        self->y += 0x200;
        py = &self->y;
        if (self->steps > 0) {
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
            s32 *pprevZ, *pprevX, *pvz;
            s32 lim;
            self->outX = *px + h - self->prevX;
            self->outY = *py + *idy;
            self->outZ = self->vz + *idz;
            dx = (*px - self->prevX) >> 12;
            dz = (self->z - self->prevZ) >> 12;
            lim = 0x3000;
            pprevX = &self->prevX;
            pprevZ = &self->prevZ;
            pvz = &self->vz;
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
            b = (u32) ~(self->unk38 >> 12) >> 31;
            if (self->unk4 != self->unk8 && (self->unk4 & 0x1000) && !(self->unk8 & 0xF000) &&
                b <= 1 && a == 1) {
                s32 v = ((*vy >> 10) + (*bias >> 8)) >> 1;
                if (v < 4)
                    v = 4;
                if (v > 18)
                    v = 18;
                *py -= v << 9;
                self->flags40 &= ~1;
            }
            p->height = h;

            Actor_UpdateShadowSpriteParts(self, p->model);
            if (self->flags40 & 2)
                *pvz = 0;
            if (self->flags40 & 1)
                *py = 0;
        }
    }
    if (p->timer > 120) {
        void (*callback)(void *);
        u32 zero;
        u32 mask = 0x800;
        if (((gEngineFlags | gScenePauseFlags) & mask) || IsRecoveryEnabled(0x100)) {
            callback = (void (*)(void *))sub_08117F6C;
            zero = 0;
        } else {
            callback = (void (*)(void *))Glove_OnActionComplete;
            zero = 0;
        }
        p->active = 1;
        p->update = callback;
        p->timer = zero;
    } else
        p->timer++;
}

#else
INCLUDE_ASM("asm/nonmatching", Glove_UpdateDefeatEffects);
#endif
