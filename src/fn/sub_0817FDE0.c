// MATCHING_CONCESSION: register-pinning
#include "global.h"

struct FDE0Header {
    u32 w[8];
};

s32 Script_SeekToKeyword(u32);
s32 Script_GetKeywordValue(u32, s32);
s32 Script_GetValue(void);
u32 Script_GetPc(void);
void SceneActors_Register(u8 *, s32);
void ResourceHeader_Relocate(void *, void *);
void *ScriptProc_Start(u32, u32);
void SceneActors_Initialize(u8 *, u32, s32, void *, u32);
void CellObject_Init(void *, void *);
void SceneBody_SetMovementProbe(void *, void *, u32, u32);
void TerrainOverlay_QueueAddActor(void *);
void Widget_InitLinked(void *, void *, s32, s32, s32, s32, s32, void *);
void Anim_SelectPaletteBank(void *, s32);
void SceneActors_StartUnmirroredAnimation(u8 *, void *, void *, s32, s32, s32);
void SpriteComponent_Init(void *);
void SpriteComponent_ConfigureCollider(void *, s32, s32, s32);
void *FrostEmitter_Create(void);
void ActorAnimation_UpdateDirectional(void);
void BossSlot2_ApplyPendingDamage(void);
void BossSlot2_InitStateDispatch(void);
void SceneActors_SetHealthLimits(u8 *, s32, s32, s32);
void ActorImpact_InitAttackCollider(u8 *);
void ActorImpact_InitBodyCollider(u8 *, s32);
void ActorImpact_InitCollider(u8 *);
void SceneActors_SetEventScripts(u8 *, s32, s32);
void SceneActors_InitConvergingParticles(u8 *);
void Shadow_Initialize(void *, void *, s32);
void ProximityVolume_InitializeCallbacks(void *);
void ProximityVolume_SetParameter(void *, s32);
void CameraFollow_Initialize(void *, u32, void *, s32);
void SceneActors_SetDamageCallback(u8 *, void *, void *, s32);
void sub_0817A7C8(void);
void ProximityVolume_TranslateBounds(void *, void *);
void Task4E01_Destroy(void);
void *TimedEffect_GetOrCreateManager(s32);

struct BossBody {
    u8 pad0[0xC];
    u8 position[8];
    u8 pad14[0x34];
};
struct BossWidget {
    u8 pad0[0x20];
    u8 shadowPart[0x28];
    u8 *tileData;
    u8 pad4C[0x14];
};
struct CameraSetup {
    u8 pad0[0x10];
    u32 timer;
    u32 distance;
};
struct BossContext {
    u8 pad0[0x30];
    s16 x, y, z;
    u8 pad36[0x832];
    u32 flags;
};
struct BossInit {
    u8 pad0[0x48];
    struct BossBody body;
    u32 pending;
    u8 pad94[0xE];
    u8 impactState;
    u8 padA3[0x1];
    u32 impactTimer;
    u8 padA8[0x3];
    u8 animationReady;
    u8 action;
    u8 subaction;
    u8 choice;
    u8 padAF[0x1];
    u8 queued;
    u8 restart;
    u8 padB2[0x1];
    u8 special;
    u8 padB4[0x10];
    u32 timer;
    u8 padC8[0x28];
    u32 eventScripts[0x4];
    void *animationCallback;
    void *damageCallback;
    void *stateCallback;
    u32 targets[0x1A];
    u8 targetCount;
    u8 pad175[0x1];
    u16 damage;
    u8 pad178[0x54];
    u8 attackCollider[0xA8];
    u16 firstThreshold;
    u16 secondThreshold;
    u8 cell[0x10];
    u8 bounds[0x24];
    struct FDE0Header resource;
    struct BossWidget widget;
    u8 sprite[0xB0];
    u8 shadow[0x40];
    void *frostEmitter;
    u8 pad420[0x8];
    u8 volume[0x3C];
    u32 signalTimer;
    u8 pad468[0xE];
    u8 signalPending;
    u8 pad477[0x69];
    u8 volumeEnabled;
    u8 volumeState;
    u8 pad4E2[0x3];
    u8 volumeTimer;
    u16 volumeFlags;
    u8 pad4E8[0x2];
    u8 mode;
    u8 pad4EB[0x7];
    s16 floorHeight;
    u8 pad4F4[0x2];
    u16 parameter;
    u16 radius;
    s16 colliderRadius;
    u16 range;
    s16 volumeParameter;
    u16 height;
    u8 pad502[0x2];
    u8 effectState;
    u8 pad505[0x1];
    u8 effectIndex;
    u8 effectCount;
    u32 effectResource;
    u8 effectEnabled;
    u8 pad50D[0x12];
    u8 active;
    u8 pad520[0xE0];
    u8 tiles0[0x200];
    u8 tiles1[0x200];
    u8 tiles2[0x200];
    u8 tiles3[0x20];
    u8 padC20[0x10];
    struct CameraSetup camera;
    u8 padC48[0x18];
    u32 firstSummon;
    u32 secondSummon;
};

static inline void SetDamageCallback(struct BossInit *p, void *callback)
{
    p->damageCallback = callback;
}
static inline void SetStateCallback(struct BossInit *p, void *callback)
{
    p->stateCallback = callback;
}

// confidence=low: boss actor setup from its script, sprite and collider init, VRAM copies
s32 sub_0817FDE0(struct BossInit *boss, u32 arg1)
{
    struct PackedPosition {
        s32 x : 16;
        s32 y : 16;
        s32 z : 16;
        s32 pad : 16;
    } pos;
    s32 mode;
    s32 count;
    register s32 i asm("r4");
    s32 sl;
    u32 flags;
    u8 *res;
    u32 *dst;
    u32 *src;
    u8 *targets;
    u32 *cameraTarget;
    u8 *volumePosition;
    u32 zero;
    struct BossInit *actor = boss;

    {
        struct BossContext *gp = *(struct BossContext **)0x02000710;
        gp->flags |= 0x400;
    }

    mode = Script_GetKeywordValue(0x74, 2);
    if (mode != 2)
        return -1;
    SceneActors_Register((u8 *)actor, 2);
    boss->parameter = Script_GetKeywordValue(0x42, 0);
    boss->special = Script_GetKeywordValue(0x69, 0);
    count = Script_GetKeywordValue(0x50, 1);
    boss->targetCount = count;
    i = 0;
    if (i < boss->targetCount) {
        targets = &boss->targetCount;
        dst = boss->targets;
        src = (u32 *)0x02000580;
        do {
            if (*src != 0) {
                u32 value = *src;
                dst[0] = value;
                dst[1] = value + 0x30;
            } else {
                dst[0] = 0;
                dst[1] = 0;
            }
            dst++;
            src++;
            i++;
        } while (i < *targets);
    }

    res = ScriptProc_Start(0xCB05, 0xDE23);
    if (res == 0)
        return -1;
    boss->resource = *(struct FDE0Header *)res;
    ResourceHeader_Relocate(&boss->resource, res);

    if (Script_SeekToKeyword(0x70)) {
        pos.x = Script_GetValue();
        pos.y = Script_GetValue();
        pos.z = Script_GetValue();
        flags = ((Script_GetValue() << 29) + 0xA0000000u) >> 24;
    } else {
        struct BossContext *gp = *(struct BossContext **)0x02000710;
        pos.x = gp->x;
        pos.y = gp->y;
        pos.z = gp->z;
        flags = 0x40;
    }
    boss->floorHeight = pos.y;

    if (Script_SeekToKeyword(0x73)) {
        sl = Script_GetValue();
        boss->firstThreshold = Script_GetValue();
        boss->secondThreshold = Script_GetValue();
        boss->damage = Script_GetValue();
    } else {
        sl = 0x3E8;
        boss->damage = 0x14;
    }

    if (Script_SeekToKeyword(0x61)) {
        boss->radius = Script_GetValue();
        boss->colliderRadius = Script_GetValue();
        boss->range = Script_GetValue();
        boss->volumeParameter = Script_GetValue();
        boss->height = Script_GetValue();
    } else {
        boss->radius = 0x10;
        boss->colliderRadius = 0x10;
        boss->range = 0x10;
        boss->volumeParameter = 0x10;
        boss->height = 0x20;
    }

    SceneActors_Initialize((u8 *)actor, arg1, mode, &pos, flags);
    CellObject_Init(boss->cell, &pos);
    SceneBody_SetMovementProbe(&actor->body, boss->cell, 0x23, 0x23);
    TerrainOverlay_QueueAddActor(&actor->body);
    Widget_InitLinked(&boss->widget, &boss->resource, 0, 0, 2, 0, 0x3C, &pos);
    Anim_SelectPaletteBank(&boss->widget, 0x34);
    SceneActors_StartUnmirroredAnimation((u8 *)actor, &boss->widget, &boss->resource, 0xF4, 1, 0);
    SpriteComponent_Init(boss->sprite);
    SpriteComponent_ConfigureCollider(boss->sprite, boss->colliderRadius, 0x40, 0x1E);
    boss->frostEmitter = FrostEmitter_Create();

    actor->mode = 0;
    actor->action = 0;
    actor->subaction = 0;
    actor->choice = 0;
    actor->timer = 0;
    actor->queued = 1;
    {
        void *callback = ActorAnimation_UpdateDirectional;
        actor->animationReady = 1;
        actor->animationCallback = callback;
    }
    actor->pending = 0;
    actor->restart = 1;
    actor->impactState = 0;
    actor->impactTimer = 0;
    SetDamageCallback(actor, BossSlot2_ApplyPendingDamage);
    SetStateCallback(actor, BossSlot2_InitStateDispatch);

    SceneActors_SetHealthLimits((u8 *)actor, sl, sl, 0);
    ActorImpact_InitAttackCollider((u8 *)boss);
    ActorImpact_InitBodyCollider((u8 *)boss, 0);
    ActorImpact_InitCollider((u8 *)boss);

    if (Script_SeekToKeyword(0x65)) {
        u32 *p;
        u32 clear;
        cameraTarget = &actor->targets[1];
        volumePosition = actor->body.position;
        p = &boss->firstSummon;
        i = 1;
        do {
            *p++ = Script_GetValue();
        } while (--i >= 0);
    } else {
        u32 *p;
        u32 clear;
        cameraTarget = &actor->targets[1];
        volumePosition = actor->body.position;
        clear = 0;
        i = 1;
        p = &boss->secondSummon;
        do {
            *p-- = clear;
        } while (--i >= 0);
    }

    if (Script_SeekToKeyword(0x63)) {
        s32 a = Script_GetValue();
        s32 b = Script_GetValue();
        SceneActors_SetEventScripts((u8 *)actor, a, b);
    } else {
        SceneActors_SetEventScripts((u8 *)actor, 0, 0);
    }

    if (Script_SeekToKeyword(0x41)) {
        u32 *p;
        i = 0;
        p = actor->eventScripts;
        for (; i <= 3 && Script_GetPc() != 0; i++)
            *p++ = Script_GetValue();
    }

    SceneActors_InitConvergingParticles((u8 *)boss);
    Shadow_Initialize(boss->shadow, (u8 *)boss + 0x2EC, 0);
    ProximityVolume_InitializeCallbacks(boss->volume);
    ProximityVolume_SetParameter(boss->volume, boss->volumeParameter);
    boss->volumeEnabled = 1;
    boss->volumeState = 0;
    boss->volumeTimer = 0;
    boss->volumeFlags = 0;

    {
        u8 *srcp;
        u8 *dstp;
        u32 off;
        u8 **tiles = (u8 **)0x030042E4;
        dstp = boss->tiles0;
        off = 0x680;
        i = 15;
        do {
            srcp = *tiles + off;
            CpuSet(srcp, dstp, 0x04000008);
            dstp += 0x20;
            off += 0x20;
        } while (--i >= 0);
        srcp = *(u8 **)0x030042E4 + 0x3240;
        dstp = boss->tiles1;
        i = 15;
        do {
            CpuSet(srcp, dstp, 0x04000008);
            dstp += 0x20;
        } while (--i >= 0);
        srcp = *(u8 **)0x030042E4 + 0x3240;
        dstp = boss->tiles2;
        i = 15;
        do {
            CpuSet(srcp, dstp, 0x04000008);
            dstp += 0x20;
        } while (--i >= 0);
        zero = 0;
        CpuSet(srcp, boss->tiles3, 0x04000008);
    }

    boss->widget.tileData = boss->tiles0;
    boss->effectIndex = zero;
    boss->effectResource = 0x34;
    boss->effectState = zero;
    boss->effectCount = 5;
    boss->effectEnabled = 1;
    CameraFollow_Initialize(&boss->camera, *cameraTarget, (u8 *)boss + 0x2EC, 1);
    {
        struct CameraSetup *camera = &boss->camera;
        u32 distance = 0x8C;
        camera->timer = zero;
        camera->distance = distance;
    }
    SceneActors_SetDamageCallback((u8 *)actor, boss->attackCollider, sub_0817A7C8, boss->damage);
    ProximityVolume_TranslateBounds(boss->bounds, volumePosition);
    Task4E01_Destroy();
    TimedEffect_GetOrCreateManager(0x11);
    boss->signalPending = zero;
    boss->signalTimer = zero;
    boss->active = zero;
    return 0;
}
