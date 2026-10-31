#ifndef GUARD_SCRIPT_KEYFRAME_POOL_H
#define GUARD_SCRIPT_KEYFRAME_POOL_H
#include "projected_display.h"
/* Two observed views of the same 0x60-byte keyframe animation record. */
union ScriptKeyframeAnimation {
    struct ProjectedDisplayEntry display;
    struct DisplayKeyframeState keyframe;
};
struct ScriptKeyframeEntry {
    u8 unknown0[0x18];
    struct ProjectedDisplayResource resource;
    union ScriptKeyframeAnimation animation;
    u16 tag, moving, conditionalVisibility, hidden, moveFrame, moveDuration;
    struct ContactPosition position, origin, target;
};
struct ScriptKeyframeManager {
    u8 task[0x18];
    u8 capacity, count, unknown1A[2];
    struct ScriptKeyframeEntry entries[1];
};
/* 0x02000498 */
extern struct ScriptKeyframeManager *gScriptKeyframeManager;
struct ScriptKeyframeEntry *ScriptKeyframe_FindFromScript(void);
void ScriptKeyframe_UpdateEntry(struct ScriptKeyframeEntry *);
u32 ScriptKeyframe_TickManager(struct ScriptKeyframeManager *);
s32 ScriptKeyframe_DestroyManager(struct ScriptKeyframeManager *);
s32 ScriptKeyframe_InitManager(struct ScriptKeyframeManager *, u32, u32, u32);
#endif
