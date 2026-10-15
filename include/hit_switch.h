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
extern struct HitSwitchManager *gUnk_02000490;
void sub_08054294(struct HitSwitchEntry *, void (*)(struct HitSwitchEntry *));
void sub_080542A4(struct HitSwitchEntry *);
void sub_08054354(struct HitSwitchEntry *);
void sub_08054394(struct HitSwitchEntry *);
void sub_0805440C(struct HitSwitchEntry *);
void sub_08054410(struct HitSwitchEntry *);
void sub_080544B4(struct HitSwitchEntry *);
void sub_08054538(struct HitSwitchEntry *);
void sub_080545A4(struct HitSwitchManager *);
s32 sub_080546B8(struct HitSwitchManager *);
s32 sub_08054728(struct HitSwitchManager *);
void sub_080547B0(struct HitSwitchEntry *);
u32 sub_08054834(struct HitSwitchManager *);
struct HitSwitchManager *sub_08054AD0(void);
#endif
