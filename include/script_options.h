#ifndef FOUNDRY_SCRIPT_OPTIONS_H
#define FOUNDRY_SCRIPT_OPTIONS_H
#include "global.h"
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
struct ScriptOptions {
    u8 unknown0[0x18];
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
};
#endif
