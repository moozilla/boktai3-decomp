#define ScriptOptions_LoadTargets sub_0811DF78
#include "global.h"
#include "script_options.h"
void Memory_Clear(void *, u32);
s32 Script_GetValue(void);
s32 Script_GetValueSafe(void);
s32 Script_SeekToKeyword(u32);
void Script_SetPc(u8 *);
void sub_0811DF78(struct ScriptOptions *options)
{
    s32 kind;
    if (Script_SeekToKeyword('t')) {
        options->targetScript = (u8 *)Script_GetValueSafe();
        Memory_Clear(options->targets, 0x48);
        Script_SetPc(options->targetScript);
        options->targetCount = 0;
        while ((kind = Script_GetValue()) >= 0 && options->targetCount <= 5) {
            options->targets[options->targetCount].kind = kind;
            options->targets[options->targetCount].parameters[0] = Script_GetValue();
            options->targets[options->targetCount].parameters[1] = Script_GetValue();
            options->targets[options->targetCount].parameters[2] = Script_GetValue();
            options->targets[options->targetCount].value = Script_GetValue();
            options->targetCount++;
        }
    }
    if (Script_SeekToKeyword('T'))
        options->targetOption = Script_GetValueSafe();
}
