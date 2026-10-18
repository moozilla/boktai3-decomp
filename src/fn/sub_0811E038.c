#define ScriptOptions_LoadWideRecords sub_0811E038
#include "global.h"
#include "script_options.h"
void Memory_Clear(void *, u32);
s32 Script_GetValue(void);
s32 Script_GetValueSafe(void);
s32 Script_SeekToKeyword(u32);
void Script_SetPc(u8 *);
void sub_0811E038(struct ScriptOptions *options)
{
    s32 kind;
    if (Script_SeekToKeyword('w')) {
        options->wideScript = (u8 *)Script_GetValueSafe();
        Memory_Clear(options->wideEntries, 0x60);
        Script_SetPc(options->wideScript);
        options->wideCount = 0;
        while ((kind = Script_GetValue()) >= 0 && options->wideCount <= 5) {
            options->wideEntries[options->wideCount].kind = kind;
            options->wideEntries[options->wideCount].value = Script_GetValue();
            options->wideEntries[options->wideCount].parameters[0] = Script_GetValue();
            options->wideEntries[options->wideCount].parameters[1] = Script_GetValue();
            options->wideEntries[options->wideCount].parameters[2] = Script_GetValue();
            options->wideEntries[options->wideCount].parameters[3] = Script_GetValue();
            options->wideEntries[options->wideCount].parameterC = Script_GetValue();
            options->wideEntries[options->wideCount].counter = 0;
            options->wideCount++;
        }
    }
    if (Script_SeekToKeyword('W'))
        options->wideOption = Script_GetValueSafe();
}
