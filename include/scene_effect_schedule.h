#ifndef GUARD_SCENE_EFFECT_SCHEDULE_H
#define GUARD_SCENE_EFFECT_SCHEDULE_H
#include "engine.h"
#include "contact_actor.h"
struct SceneScheduledEffect {
    u8 unknown0[0x13e];
    s16 state;
};
struct SceneEffectScheduleEntry {
    struct ContactPosition position, offset;
    s16 randomValue, scaleValue;
    u16 id, flags;
    u8 kind, style;
    s16 index;
    struct SceneScheduledEffect *effect;
};
struct SceneEffectSchedule {
    u8 task[0x18];
    struct SceneEffectScheduleEntry entries[12];
    u32 occupiedMask;
};
extern struct SceneEffectSchedule *gSceneEffectSchedule;
extern u32 gUnk_030053F0, gUnk_02000560;
struct SceneEffectScheduleEntry *SceneEffectSchedule_Allocate(struct SceneEffectSchedule *);
s32 SceneEffectSchedule_Tick(struct SceneEffectSchedule *);
void SceneEffectSchedule_Destroy(void *);
s32 SceneEffectSchedule_Initialize(struct SceneEffectSchedule *, u32);
struct SceneEffectSchedule *SceneEffectSchedule_Create(u32);
struct SceneScheduledEffect *SceneScheduledEffect_Create(const struct ContactPosition *, u32, s32, u32, u32, s32,
                                          s32, s32, s32, s32, u32);
#endif
