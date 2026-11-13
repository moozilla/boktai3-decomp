#ifndef GUARD_SCRIPT_KEYFRAME_POOL_H
#define GUARD_SCRIPT_KEYFRAME_POOL_H
#include "projected_display.h"
/* Two observed views of the same 0x60-byte keyframe animation record. */
union ScriptKeyframeAnimation {
    struct ProjectedDisplayEntry display;
    struct DisplayKeyframeState keyframe;
};
struct ScriptKeyframeEntry {
    u8 unknown0[0x18]; /* reserved prefix; no accesses observed in this unit */
    struct ProjectedDisplayResource resource;                                /* 0x18 */
    union ScriptKeyframeAnimation animation;                                 /* 0x38 */
    u16 tag, moving, conditionalVisibility, hidden, moveFrame, moveDuration; /* 0x98 */
    struct ContactPosition position, origin, target;                         /* 0xA4 */
};
struct ScriptKeyframeManager {
    struct Task task;                      /* 0x00 */
    u8 capacity, count, unknown1A[2];      /* 0x18 */
    struct ScriptKeyframeEntry entries[1]; /* 0x1C: variable-length tail */
};
/* 0x02000498 */
extern struct ScriptKeyframeManager *gScriptKeyframeManager;
struct ScriptKeyframeEntry *ScriptKeyframe_FindFromScript(void);
void ScriptKeyframe_UpdateEntry(struct ScriptKeyframeEntry *);
u32 ScriptKeyframe_TickManager(struct ScriptKeyframeManager *);
s32 ScriptKeyframe_DestroyManager(struct ScriptKeyframeManager *);
s32 ScriptKeyframe_InitManager(struct ScriptKeyframeManager *, u32, u32, u32);
void ScriptKeyframe_SetFrameFromScript(void);
void ScriptKeyframe_SetMirrorFromScript(void);
void ScriptKeyframe_HideFromScript(void);
void ScriptKeyframe_ShowFromScript(void);
void ScriptKeyframe_SetPriorityFromScript(void);
void ScriptKeyframe_SetTargetFromScript(void);
void ScriptKeyframe_AddFromScript(void);
struct ScriptKeyframeManager *ScriptKeyframe_CreateManagerFromScript(u32, u32);
void Widget_InitLinked(void *, void *, u32, u32, u32, u32, s32, const void *);
#endif
