#include "global.h"
#include "render.h"
#include "game_state.h"

// confidence=low: Hresvelgr setup: reads script keywords into the 0x358-0x372 parameter block, creates the pools, sets callbacks and animation state.
extern u32 gUnknown_02000580[4];
struct F2C80Block {
    u32 w[8];
};
s32 Script_GetKeywordValue(s32, s32);
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
void *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(void *, void *);
void SceneActors_Register(void *, s32);
s32 SceneActors_Initialize(void *, void *, s32, void *, s32);
void Hresvelgr_CreateShotPool(s32, s32, s32);
void EffectPool_Create(s32, s32, s32);
void Hresvelgr_CreateProjectilePool(s32, s32, s32);
void Hresvelgr_CreateEffectPool(void);
void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void SceneActors_SetHealthLimits(void *, s32, s32, s32);
void Hresvelgr_InitializeCollider(void *, u32);
void ColliderActor_InitBodyCollider(void *, s32);
void ColliderActor_InitLowerBodyCollider(void *, s32);
void SceneActors_SetEventScripts(void *, s32, s32);
void SceneActors_SetDamageCallback(void *, void *, void *, s32);
void Hresvelgr_InitializeRadialOffsets(void *);
void ActorAnimation_ResetSequence(void *);
void ScaledShadow_Initialize(void *, void *, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void ScaledShadow_SetGraphic(void *, s32, s32);
void Hresvelgr_ApplyContactDamage(void);
s32 Hresvelgr_ApplyRequestedAction(void *);
void Hresvelgr_BeginDamageReaction(void);
void Hresvelgr_BeginRestAnimation(void);
void Hresvelgr_OnActionTwentyEight(void);
struct HresSprite {
    u8 pad0[0x20];
    u8 position[0x1A];
    u16 frameData;
    u8 pad3C[0xC];
    void *tiles;
    u8 pad4C[0x68];
};
struct HresvelgrInit {
    u8 pad0[0x90];
    u32 pending;
    u8 pad94[0xE];
    u8 impactState;
    u8 padA3[0x1];
    u32 impactTimer;
    u8 direction4;
    u8 padA9[0x2];
    u8 animationId;
    u8 action;
    u8 subaction;
    u8 choice;
    u8 padAF[0x1];
    u8 queued;
    u8 restart;
    u8 padB2[0x12];
    u32 timer;
    u8 padC8[0x28];
    u32 eventScripts[0x4];
    void *animationCallback;
    void *damageCallback;
    void *stateCallback;
    u32 targets[0x1A];
    struct F2C80Block resource;
    struct HresSprite sprite;
    u8 attackCollider[0xE3];
    u8 facingSide;
    u8 pad32C[0x2C];
    u16 parameter358;
    u16 parameter35A;
    u16 parameter35C;
    s16 shotCount;
    s16 shotRange;
    s16 shotSpeed;
    s16 effectCount;
    s16 effectRange;
    s16 effectLifetime;
    s16 projectileCount;
    s16 projectileRange;
    s16 projectileLifetime;
    u16 parameter370;
    u16 parameter372;
    u16 attackDamage;
    s16 lowerBodyDamage;
    s16 bodyDamage;
    u16 parameter37A;
    u16 targetCount;
    u16 counter37E;
    u16 counter380;
    u8 state382;
    u8 pad383[0x31];
    u8 shadow[0x6C];
    u32 timer420;
    u8 paletteBuffer[0x2E0];
    u8 tileMap[0xE0];
    u32 scriptParameter;
};
static inline void SetDamageCallback(struct HresvelgrInit *p, void *callback)
{
    p->damageCallback = callback;
}
static inline void SetStateCallback(struct HresvelgrInit *p, void *callback)
{
    p->stateCallback = callback;
}
static inline void SetAction(struct HresvelgrInit *p, u32 value)
{
    p->action = value;
}
static inline void SetAnimation(struct HresvelgrInit *p, void *callback, u32 id)
{
    p->animationId = id;
    p->animationCallback = callback;
}
// confidence=low: Hresvelgr setup: reads script keywords into the 0x358-0x372 parameter block, creates the pools, sets callbacks and animation state.
s32 sub_081A2C80(struct HresvelgrInit *self, void *arg1)
{
    struct HresvelgrInit *actor = self;
    struct PackedPosition {
        s32 x : 16, y : 16, z : 16, pad : 16;
    } pk;
    s32 mode;
    s32 count;
    s32 i;
    s32 sbVal;
    s32 slVal;
    s32 v;
    u8 r5;
    u8 *hdr;
    u32 *w;
    u32 src;
    u32 off;
    u32 *players;
    u16 *targetCount;
    u8 *paletteSource;
    u8 *paletteDestination;
    void **paletteBank;

    mode = Script_GetKeywordValue(0x74, 1);
    if (mode != 1)
        return -1;
    SceneActors_Register(actor, 1);
    count = Script_GetKeywordValue(0x50, 1);
    actor->targetCount = count;
    i = 0;
    if (i < count) {
        targetCount = &actor->targetCount;
        w = actor->targets;
        players = gUnknown_02000580;
        do {
            src = *players;
            if (src) {
                w[0] = src;
                w[1] = src + 0x30;
            } else {
                w[0] = 0;
                w[1] = 0;
            }
            w++;
            players++;
            i++;
        } while (i < *targetCount);
    }
    hdr = ScriptProc_Start(0xCB05, 0xF771);
    if (!hdr)
        return -1;
    self->resource = *(struct F2C80Block *)hdr;
    ResourceHeader_Relocate(&self->resource, hdr);

    if (Script_SeekToKeyword(0x70)) {
        pk.x = Script_GetValue();
        pk.y = Script_GetValue();
        pk.z = Script_GetValue();
        v = Script_GetValue();
        r5 = (u8)(((u32)(v << 29) + 0xA0000000u) >> 24);
    } else {
        pk.x = gGameState->positionX;
        pk.y = gGameState->positionY;
        pk.z = gGameState->positionZ;
        r5 = 0x40;
    }

    if (Script_SeekToKeyword(0x73)) {
        sbVal = Script_GetValue();
        self->lowerBodyDamage = Script_GetValue();
        self->bodyDamage = Script_GetValue();
        self->attackDamage = Script_GetValue();
        slVal = Script_GetValue();
        self->parameter37A = Script_GetValue();
    } else {
        sbVal = 1;
        self->lowerBodyDamage = 0;
        self->bodyDamage = 0;
        self->attackDamage = 100;
        slVal = 0;
        self->parameter37A = 0;
    }

    if (Script_SeekToKeyword(0x61)) {
        self->parameter358 = Script_GetValue();
        self->parameter35A = Script_GetValue();
        self->parameter35C = Script_GetValue();
        self->shotCount = Script_GetValue();
        self->shotRange = Script_GetValue();
        self->shotSpeed = Script_GetValue();
        self->effectCount = Script_GetValue();
        self->effectRange = Script_GetValue();
        self->effectLifetime = Script_GetValue();
        self->projectileCount = Script_GetValue();
        self->projectileRange = Script_GetValue();
        self->projectileLifetime = Script_GetValue();
        self->parameter370 = Script_GetValue();
        self->parameter372 = Script_GetValue();
    } else {
        self->parameter358 = 0x32;
        self->parameter35A = 0x50;
        self->parameter35C = 0x3C;
        self->shotCount = 8;
        self->shotRange = 0x28;
        self->shotSpeed = 0xF;
        self->effectCount = 0x50;
        self->effectRange = 0x50;
        self->effectLifetime = 0x3C;
        self->projectileCount = 0x80;
        self->projectileRange = 0x50;
        self->projectileLifetime = 0x3C;
        self->parameter370 = 0x78;
        self->parameter372 = 8;
    }

    Hresvelgr_CreateShotPool(self->shotCount, self->shotRange, self->shotSpeed);
    EffectPool_Create(self->effectCount, self->effectRange, self->effectLifetime);
    Hresvelgr_CreateProjectilePool(self->projectileCount, self->projectileRange,
                                   self->projectileLifetime);
    Hresvelgr_CreateEffectPool();
    SceneActors_Initialize(actor, arg1, mode, &pk, r5);
    self->facingSide = (u8)(actor->direction4 >> 1);
    Widget_InitLinked(&self->sprite, &self->resource, 0, 0, 2, 0, 0x3C, &pk);
    SceneActors_StartUnmirroredAnimation(actor, &self->sprite, &self->resource, 0x18, 1, 0);
    SetDamageCallback(actor, Hresvelgr_ApplyContactDamage);
    SetStateCallback(actor, Hresvelgr_ApplyRequestedAction);
    SceneActors_SetHealthLimits(actor, sbVal, sbVal, self->lowerBodyDamage);
    Hresvelgr_InitializeCollider(self, (u16)(u32)arg1);
    ColliderActor_InitBodyCollider(self, self->bodyDamage);
    ColliderActor_InitLowerBodyCollider(self, self->lowerBodyDamage);
    self->scriptParameter = Script_GetKeywordValue(0x45, 0);
    if (Script_SeekToKeyword(0x63)) {
        s32 ev1 = Script_GetValue();
        s32 ev2 = Script_GetValue();
        SceneActors_SetEventScripts(actor, ev1, ev2);
    } else {
        SceneActors_SetEventScripts(actor, 0, 0);
    }
    if (Script_SeekToKeyword(0x41)) {
        u32 *ev = actor->eventScripts;
        for (i = 3; i >= 0; i--)
            *ev++ = Script_GetValue();
    }
    paletteBank = (void **)0x030042E4;
    paletteDestination = self->paletteBuffer;
    off = 0x5E00;
    for (i = 15; i >= 0; i--) {
        CpuSet((u8 *)*paletteBank + off, paletteDestination, 0x04000008);
        paletteDestination += 0x20;
        off += 0x20;
    }
    {
        for (i = 0; i <= 0x6F; i++) {
            s32 tileOffset = ((i & 0xF) << 1) + ((i >> 4) << 5);
            u8 *tileMap = self->tileMap;
            u16 colour = 0x1084;
            *(u16 *)(tileMap + tileOffset) = colour;
        }
    }
    self->sprite.frameData = 0x2F0;
    self->sprite.tiles = self->paletteBuffer;
    ScaledShadow_Initialize(self->shadow, self->sprite.position, 0x40, 0x40, 0x40, 0x40, 0x2000, 1,
                            6, 0, 0);
    ScaledShadow_SetGraphic(self->shadow, 0x1C1B, 7);
    self->counter37E = 0;
    self->counter380 = 0;
    self->state382 = 0;
    SceneActors_SetDamageCallback(actor, self->attackCollider,
                                  (void *)Hresvelgr_BeginDamageReaction, slVal);
    Hresvelgr_InitializeRadialOffsets(self);
    ActorAnimation_ResetSequence(self);
    self->timer420 = 0;

    if (Script_SeekToKeyword(0x44)) {
        s32 t = Script_GetValue();
        switch (t) {
            case 0:
                SetAction(actor, 2);
                actor->subaction = 0;
                actor->choice = 1;
                actor->timer = 0;
                actor->queued = 1;
                SetAnimation(actor, Hresvelgr_BeginRestAnimation, 0x1D);
                actor->pending = 0;
                actor->restart = 1;
                actor->impactState = 0;
                actor->impactTimer = 0;
                break;
            case 1:
                SetAction(actor, 2);
                actor->subaction = 0;
                actor->choice = 0;
                actor->timer = 0;
                actor->queued = 1;
                SetAnimation(actor, Hresvelgr_OnActionTwentyEight, 0x1C);
                actor->pending = 0;
                actor->restart = 1;
                actor->impactState = 0;
                actor->impactTimer = 0;
        }
    }
    return 0;
}
