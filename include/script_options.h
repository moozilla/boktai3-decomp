#ifndef GUARD_SCRIPT_OPTIONS_H
#define GUARD_SCRIPT_OPTIONS_H
#include "engine.h"
#include "scene_pool.h"
#include "script_reference.h"
#include "sun_clock.h"
struct ScriptTargetEntry {
    u8 kind, unknown1;
    u16 parameters[3];
    s32 value;
};
struct ScriptOptionEntry {
    u8 kind, value;
    u16 parameter;
    u8 mode, unknown5[3];
};
struct ScriptWideEntry {
    u8 kind, value;
    u16 parameters[4];
    u16 counter, parameterC;
    u8 unknownE[2];
};
struct ScriptShortEntry {
    u8 kind, value;
    u16 parameters[4];
    u8 unknownA[2];
};
struct ScriptByteEntry {
    u8 kind, category, parameterA, parameterB;
    u16 value;
    u8 unknown6[2];
};
struct MotionTween {
    s32 targetValue, unknown4, targetCoordinate, unknownC;
    u8 valueFrames, coordinateFrames, valueDelay, coordinateDelay;
};
/* Script-loaded modifier tables; task allocation is 0x23C bytes. */
struct ScriptOptions {
    struct Task task;
    u8 *entryScript;
    u32 option;
    struct ScriptOptionEntry entries[24];
    s32 entryCount;
    u8 *targetScript;
    u32 targetOption;
    struct ScriptTargetEntry targets[6];
    s32 targetCount;
    u8 *wideScript;
    u32 wideOption;
    struct ScriptWideEntry wideEntries[6];
    s32 wideCount;
    u8 *shortScript;
    u32 shortOption;
    struct ScriptShortEntry shortEntries[6];
    s32 shortCount;
    u8 *byteScript; /* +0x1F8 */
    s32 byteOption;
    struct ScriptByteEntry byteEntries[6];
    s32 byteCount;
    u8 unknown234[8];
};
u32 GameState_SetChallengeCompletionFlag(u32);
void ClockDisplay_DrawBaseTime(s32);
void StateFlags_SetCategoryBit(u32, u32);
u32 StateFlags_GetCategoryBit(u32, u32);
void ScriptOptions_LoadEntries(struct ScriptOptions *);
void ScriptOptions_LoadTargets(struct ScriptOptions *);
void ScriptOptions_LoadWideRecords(struct ScriptOptions *);
void ScriptOptions_LoadShortRecords(struct ScriptOptions *);
void ScriptRecord_ReadEntries(struct ScriptOptions *);
void Modifier_LoadTablesFromScript(struct ScriptOptions *);
s32 Modifier_InitTableTask(struct ScriptOptions *);
void Modifier_OnTableTaskUpdate(void);
u32 Modifier_OnTableTaskDestroy(void);
s32 Script_GetValueSafe(void);
void Script_SetPc(u8 *);
void Text_DrawNumber(s32, s32, s32, s32, s32, s32, s32);
#endif
