#include "global.h"
#include "render.h"

/* confidence=low: Initialize the Vampire actor from script options and create its visual and contact objects. */

struct VampireInitPlayer;
extern struct VampireInitPlayer *gUnknown_02000580[];
extern void *gGameState;
extern const u8 gUnk_0824DC78[];
void *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(void *, void *);
u32 Script_SeekToKeyword(u32);
u32 Script_GetValue(void);
u32 Script_GetKeywordValue(u32, u32);
u32 Script_GetPc(void);
void SceneActors_Register(void *, u32);
void SceneActors_Initialize(void *, u32, u32, void *, u32);
void SceneBody_SetPositionOffset(void *, const void *);
void Widget_InitLinked(void *, void *, u32, u32, u32, u32, s32, const void *);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void SceneActors_SetHealthLimits(void *, u32, u32, u32);
void SceneActorGraphics_InitializeBody(void *);
void SceneActorGraphics_InitializeOverlay(void *, u32);
void SceneActors_SetEventScripts(void *, u32, u32);
void ScaledShadow_Initialize(void *, void *, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void ScaledShadow_SetGraphic(void *, u32, u32);
void Math_SetMotionPosition(void *, void *);
void *BackgroundFlash_Create(s16, s16);
void *ContactSwarm_Create(void);
void *ContactActorPool_Create(u32, u32, u32);
void CameraFollow_Initialize(void *, void *, void *, u32);
void SceneActors_SetDamageCallback(void *, void *, void *, u32);
void CpuSet(const void *, void *, u32);
void Vampire_ApplyPendingDamage(void);
void SceneActors_TryApplyRequestedState(void);
void SceneActorAnimation_UpdateDirectionalCallback(void);
void SceneActorAnimation_UpdateEightDirectionCallback(void);
void SceneActors_UpdateStationaryReaction(void);
void sub_08022B18(void);

struct VampireInitPosition {
    s32 x : 16, y : 16, z : 16, pad : 16;
};

struct VampireInitPlayer {
    u8 pad0[0x30];
    struct VampireInitPosition position;
};

struct VampireResourceHeader {
    u32 words[8];
};

struct VampireInitCamera {
    u8 pad0[0x10];
    u32 angle;
    u32 distance;
    u8 pad18[0xc];
};
struct VampireInitBase {
    u8 pad0[0x48];
    u8 sprite[0x48];
    u32 timer;
    u8 pad94[0xe];
    u8 reset;
    u8 padA3;
    u32 pending;
    u8 padA8[3];
    u8 state;
    u8 animationMode;
    u8 animationPhase;
    u8 animationEntry;
    u8 padAF;
    u8 animationChanged;
    u8 changed;
    u8 padB2;
    u8 ambientMode;
    u8 padB4[0x10];
    u32 animationTimer;
    u8 padC8[0x28];
    u32 scriptValues[4];
    void *callback;
    void *damageCallback;
    void *requestedStateCallback;
    void *targetWords[26];
    u8 targetCount;
    u8 pad175;
    u16 colliderExtra;
    u8 body[0x54];
    u8 overlay[0x54];
    struct VampireResourceHeader resource;
    u8 animation[0x20];
    struct VampireInitPosition position;
    u8 pad268[0x20];
    void *palette;
    u8 pad28C[0x14];
    u8 shadow[0x6c];
    u8 motion[0x46];
    s16 initialHeight;
    u8 pad354[4];
    s32 kind;
};
struct VampireInit {
    struct VampireInitBase base;
    s16 tuning[12];
    u16 reactionLength;
    s16 hitThreshold;
    u16 flashOn, flashOff;
    u16 fadeOn, fadeOff;
    u16 effectId;
    u8 pad382[2];
    u8 damageGuard;
    u8 pad385[3];
    u32 eventMode;
    u8 blendAmount;
    u8 blendDescending;
    u8 blendMode;
    u8 blendShift;
    u32 blendResource;
    u8 trackingEnabled;
    u8 pad395[0x17];
    u32 damageFlags;
    u8 pad3B0[9];
    u8 flashState;
    u8 pad3BA[2];
    u32 flashPeriod;
    u32 flashFrames;
    u8 paletteDefault[0x200];
    u8 paletteNormal[0x200];
    u8 paletteSpecial[0x200];
    u8 pad9C4[0x10];
    void *contactSwarm;
    void *contactPool;
    void *backgroundFlash;
    struct VampireInitCamera camera;
};
struct VampireInitGameState {
    u8 pad0[0x30];
    s16 x, y, z;
};

static inline void VampireInit_SetHeight(struct VampireInit *p, s16 height)
{
    p->base.initialHeight = height;
}
static inline void VampireInit_ConfigureAnimation(struct VampireInitBase *p, u8 mode, u8 entry)
{
    p->animationMode = mode;
    p->animationPhase = 0;
    p->animationEntry = entry;
    p->animationTimer = 0;
    p->animationChanged = 1;
}
static inline void VampireInit_SetState(struct VampireInitBase *p, void *callback, int state)
{
    p->state = state;
    p->callback = callback;
    p->timer = 0;
    p->changed = 1;
    p->reset = 0;
    p->pending = 0;
}

static inline void VampireInit_SetDamageHandler(struct VampireInitBase *p, void *callback)
{
    p->damageCallback = callback;
}
static inline void VampireInit_SetRequestedStateHandler(struct VampireInitBase *p, void *callback)
{
    p->requestedStateCallback = callback;
}

static inline void VampireInit_SetCameraOrbit(struct VampireInitCamera *camera, u32 angle,
                                              u32 distance)
{
    camera->angle = angle;
    camera->distance = distance;
}

static inline u32 VampireInit_ForcedDamageFlag(u32 flags)
{
    return flags & 1;
}

int sub_08028F78(struct VampireInit *s, u32 id)
{
    struct VampireInitBase *base = &s->base;
    u32 actorType;
    s32 i, n;
    struct VampireResourceHeader *resource;
    u32 *dst;
    const u8 *palette;
    struct VampireInitPosition pos;
    struct VampireInitPosition pos2;
    s32 mode;
    u32 health;
    u32 facing;

    actorType = Script_GetKeywordValue(0x74, 0);
    if (actorType)
        return -1;
    SceneActors_Register(base, 0);
    base->kind = Script_GetKeywordValue(0x42, 0);
    base->ambientMode = Script_GetKeywordValue(0x69, 0);
    n = Script_GetKeywordValue(0x50, 1);
    base->targetCount = n;
    i = 0;
    if (i < n) {
        u8 *count = &base->targetCount;
        void **dst = base->targetWords;
        struct VampireInitPlayer **src = gUnknown_02000580;
        /* Each entry advances one word; its position pointer shares the next slot. */
        do {
            struct VampireInitPlayer *q = *src;
            if (q) {
                dst[0] = q;
                dst[1] = &q->position;
            } else {
                dst[0] = 0;
                dst[1] = 0;
            }
            dst++;
            src++;
            i++;
        } while (i < *count);
    }

    resource = ScriptProc_Start(0xCB05, 0xF761);
    if (!resource)
        return -1;
    s->base.resource = *resource;
    ResourceHeader_Relocate(&s->base.resource, resource);

    if (Script_SeekToKeyword(0x70)) {
        pos.x = Script_GetValue();
        pos.y = Script_GetValue();
        pos.z = Script_GetValue();
        facing = ((Script_GetValue() << 29) + 0xA0000000) >> 24;
    } else {
        struct VampireInitGameState *cfg = gGameState;
        pos.x = (u16)cfg->x;
        pos.y = (u16)cfg->y;
        pos.z = (u16)cfg->z;
        facing = 0x40;
    }
    VampireInit_SetHeight(s, pos.y);

    if (Script_SeekToKeyword(0x72)) {
        pos2.x = Script_GetValue();
        pos2.y = Script_GetValue();
        pos2.z = Script_GetValue();
    } else {
        pos2.x = 0x600;
        pos2.y = 0x100;
        pos2.z = 0x600;
    }
    pos2.y = s->base.initialHeight;
    Math_SetMotionPosition(s->base.motion, &pos2);

    if (Script_SeekToKeyword(0x73)) {
        health = Script_GetValue();
        s->base.colliderExtra = Script_GetValue();
        s->reactionLength = Script_GetValue();
        s->hitThreshold = Script_GetValue();
    } else {
        health = 0x3E8;
        s->base.colliderExtra = 0x14;
        s->reactionLength = 0x1A4;
        s->hitThreshold = 5;
    }

    if (Script_SeekToKeyword(0x61)) {
        s->tuning[0] = Script_GetValue();
        s->tuning[1] = Script_GetValue();
        s->tuning[2] = Script_GetValue();
        s->tuning[3] = Script_GetValue();
        s->tuning[4] = Script_GetValue();
        s->tuning[5] = Script_GetValue();
        s->tuning[6] = Script_GetValue();
        s->tuning[7] = Script_GetValue();
        s->tuning[8] = Script_GetValue();
        s->tuning[9] = Script_GetValue();
        s->tuning[10] = Script_GetValue();
        s->tuning[11] = Script_GetValue();
    } else {
        s->tuning[0] = 40;
        s->tuning[1] = 50;
        s->tuning[2] = 60;
        s->tuning[3] = 90;
        s->tuning[4] = 80;
        s->tuning[5] = 60;
        s->tuning[6] = 60;
        s->tuning[7] = 80;
        s->tuning[8] = 50;
        s->tuning[9] = 45;
        s->tuning[10] = 1;
        s->tuning[11] = 15;
    }

    SceneActors_Initialize(base, id, actorType, &pos, facing);
    SceneBody_SetPositionOffset(base->sprite, gUnk_0824DC78);
    Widget_InitLinked(s->base.animation, &s->base.resource, 0, 0, 2, 0, 60, &pos);

    SceneActors_StartUnmirroredAnimation(base, s->base.animation, &s->base.resource, 5, 1, 0);

    if (base->ambientMode) {
        mode = Script_GetKeywordValue(0x44, 0);
        switch (mode) {
            case 0:
                VampireInit_ConfigureAnimation(base, 2, 1);
                VampireInit_SetState(base, (void *)SceneActorAnimation_UpdateDirectionalCallback,
                                     1);
                break;
            case 1:
                VampireInit_ConfigureAnimation(base, 2, 2);
                VampireInit_SetState(base, (void *)SceneActorAnimation_UpdateEightDirectionCallback,
                                     2);
                break;
            case 2:
                VampireInit_ConfigureAnimation(base, 2, 0);
                VampireInit_SetState(base, (void *)SceneActors_UpdateStationaryReaction, 26);
                break;
            default:
                break;
        }
    } else {
        VampireInit_ConfigureAnimation(base, 0, 0);
        VampireInit_SetState(base, (void *)SceneActorAnimation_UpdateDirectionalCallback, 1);
    }

    VampireInit_SetDamageHandler(base, (void *)Vampire_ApplyPendingDamage);
    VampireInit_SetRequestedStateHandler(base, (void *)SceneActors_TryApplyRequestedState);
    SceneActors_SetHealthLimits(base, (u32)health, (u32)health, s->base.colliderExtra);
    SceneActorGraphics_InitializeBody(s);
    SceneActorGraphics_InitializeOverlay(s, s->base.colliderExtra);

    if (Script_SeekToKeyword(0x63)) {
        u32 a = Script_GetValue();
        u32 b = Script_GetValue();
        SceneActors_SetEventScripts(base, a, b);
        s->eventMode = Script_GetValue();
    } else {
        SceneActors_SetEventScripts(base, 0, 0);
        s->eventMode = 0;
    }

    if (Script_SeekToKeyword(0x41)) {
        for (i = 0; i <= 3 && Script_GetPc(); i++) {
            base->scriptValues[i] = Script_GetValue();
        }
    }

    if (Script_SeekToKeyword(0x43)) {
        s->flashPeriod = Script_GetValue();
        s->flashOn = Script_GetValue();
        s->flashOff = Script_GetValue();
    } else {
        s->flashPeriod = 0x258;
        s->flashOn = 0x32;
        s->flashOff = 0x32;
    }
    s->flashFrames = 0;
    s->flashState = 0;

    if (Script_SeekToKeyword(0x66)) {
        s->fadeOn = Script_GetValue();
        s->fadeOff = Script_GetValue();
    } else {
        s->fadeOn = 0x1E;
        s->fadeOff = 0x1E;
    }

    s->effectId = Script_GetKeywordValue(0x53, 0);
    s->damageFlags = 0;
    {
        u8 initialDamageFlag = Script_GetKeywordValue(0x4D, 1) != 0;
        u32 enabledFlags;
        s->damageFlags = initialDamageFlag;
        enabledFlags = VampireInit_ForcedDamageFlag(s->damageFlags);
        s->damageFlags = Script_GetKeywordValue(0x54, 1) ? (2 | enabledFlags) : initialDamageFlag;
    }
    {
        u32 *flags = &s->damageFlags;
        u32 v = *flags;
        *flags = Script_GetKeywordValue(0x48, 0) ? (4 | v) : v;
    }

    ScaledShadow_Initialize(s->base.shadow, &s->base.position, 0x40, 0x40, 0x40, 0x40, 0x1900, 1, 6,
                            1, 0);
    ScaledShadow_SetGraphic(s->base.shadow, 0x1C1B, 7);

    {
        u8 **bank = &gPaletteBankData;
        u32 off;
        dst = (u32 *)(s->paletteDefault);
        off = 0x66A0;
        i = 15;
        do {
            palette = *bank + off;
            CpuSet(palette, dst, 0x04000008);
            dst += 8;
            off += 0x20;
        } while ((s32)--i >= 0);
    }
    {
        palette = gPaletteBankData + 0x6720;
        dst = (u32 *)(s->paletteNormal);
        i = 15;
        do {
            CpuSet(palette, dst, 0x04000008);
            dst += 8;
        } while ((s32)--i >= 0);
    }
    {
        palette = gPaletteBankData + 0x6700;
        dst = (u32 *)(s->paletteSpecial);
        i = 15;
        do {
            CpuSet(palette, dst, 0x04000008);
            dst += 8;
        } while ((s32)--i >= 0);
    }

    s->base.palette = s->paletteDefault;
    s->blendMode = 0;
    s->blendResource = 0x335;
    s->blendAmount = 0;
    s->blendShift = 5;

    s->backgroundFlash = BackgroundFlash_Create(s->tuning[10], s->tuning[11]);
    if (!s->backgroundFlash)
        return -1;
    s->contactSwarm = ContactSwarm_Create();
    if (!s->contactSwarm)
        return -1;
    s->contactPool = ContactActorPool_Create(s->base.targetCount, s->tuning[9], 0x64);
    if (!s->contactPool)
        return -1;

    s->trackingEnabled = 1;
    s->damageGuard = 4;
    CameraFollow_Initialize(&s->camera, base->targetWords[1], &s->base.position, 1);
    VampireInit_SetCameraOrbit(&s->camera, 0, 0x8C);
    SceneActors_SetDamageCallback(base, s->base.overlay, (void *)sub_08022B18, 0);
    return 0;
}
