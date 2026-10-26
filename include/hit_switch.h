#ifndef GUARD_HIT_SWITCH_H
#define GUARD_HIT_SWITCH_H
#include "scene_object.h"
/* Tentative identity: hit-counted switches that alter a terrain cell. */
struct HitSwitchSprite {
    u32 flags;
    u8 unknown4[3], renderMode;
    u8 unknown8[8];
    u16 frame;
    u8 unknown12[10];
    struct SceneVec position;
    u8 unknown24[8];
};
struct HitSwitchEntry {
    struct HitSwitchSprite sprite;
    struct SceneAnimation animation;
    struct SceneCollider collider;
    struct SceneBody body;
    struct SceneVec world;
    u8 kind, bodyEnabled;
    u16 collisionId;
    s16 tag;
    u16 cellIndex, frame, bobPeriod, hitCount, requiredHits;
    u8 hitCooldown, unknownFD, completed, cellRegistered;
    struct SceneCellRef cellRef;
    void (*update)(struct HitSwitchEntry *);
    u32 completedScriptId;
};
struct HitSwitchManager {
    u8 task[0x18];
    u8 count, hitPending, unknown1A, flash;
    u16 palette[16];
    struct HitSwitchEntry entries[8];
};
struct HitSwitchScriptArgs {
    u32 count : 16;
    u32 unused : 16;
    s32 *values;
};
extern struct HitSwitchManager *gHitSwitchManager;
void HitSwitch_SetUpdate(struct HitSwitchEntry *, void (*)(struct HitSwitchEntry *));
void HitSwitch_RegisterCompletedCell(struct HitSwitchEntry *);
void HitSwitch_UpdateStatic(struct HitSwitchEntry *);
void HitSwitch_AnimateStaticCompletion(struct HitSwitchEntry *);
void HitSwitch_TickCompletedStatic(struct HitSwitchEntry *);
void HitSwitch_UpdateBobbing(struct HitSwitchEntry *);
void HitSwitch_AnimateBobbingCompletion(struct HitSwitchEntry *);
void HitSwitch_UpdateCompletedBobbing(struct HitSwitchEntry *);
void HitSwitch_UpdateSharedFlash(struct HitSwitchManager *);
s32 HitSwitch_UpdateManager(struct HitSwitchManager *);
s32 HitSwitch_DestroyManager(struct HitSwitchManager *);
void HitSwitch_InitCollider(struct HitSwitchEntry *);
void HitSwitch_InitEntryFromScript(struct HitSwitchManager *);
struct HitSwitchManager *HitSwitch_CreateManager(void);
#endif
