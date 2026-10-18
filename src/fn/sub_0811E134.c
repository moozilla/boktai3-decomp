#define ScriptOptions_LoadShortRecords sub_0811E134
#include "global.h"
#include "script_options.h"
void Memory_Clear(void *, u32);
s32 Script_GetValue(void);
s32 Script_GetValueSafe(void);
s32 Script_SeekToKeyword(u32);
void Script_SetPc(u8 *);
void sub_0811E134(struct ScriptOptions *options)
{
    s32 kind;
    if (Script_SeekToKeyword('s')) {
        options->shortScript = (u8 *)Script_GetValueSafe();
        Memory_Clear(options->shortEntries, 0x48);
        Script_SetPc(options->shortScript);
        options->shortCount = 0;
        while ((kind = Script_GetValue()) >= 0 && options->shortCount <= 5) {
            options->shortEntries[options->shortCount].kind = kind;
            options->shortEntries[options->shortCount].value = Script_GetValue();
            options->shortEntries[options->shortCount].parameters[0] = Script_GetValue();
            options->shortEntries[options->shortCount].parameters[1] = Script_GetValue();
            options->shortEntries[options->shortCount].parameters[2] = Script_GetValue();
            options->shortEntries[options->shortCount].parameters[3] = Script_GetValue();
            options->shortCount++;
        }
    }
    if (Script_SeekToKeyword('S'))
        options->shortOption = Script_GetValueSafe();
}
