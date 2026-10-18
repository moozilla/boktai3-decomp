#include "global.h"
#include "script_options.h"
void Memory_Clear(void *, u32);
s32 Script_GetValue(void);
s32 Script_GetValueSafe(void);
s32 Script_SeekToKeyword(u32);
void Script_SetPc(u8 *);
void ScriptOptions_LoadEntries(struct ScriptOptions *options)
{
    s32 kind;
    if (Script_SeekToKeyword('o')) {
        options->entryScript = (u8 *)Script_GetValueSafe();
        Memory_Clear(options->entries, 0xC0);
        Script_SetPc(options->entryScript);
        options->entryCount = 0;
        while ((kind = Script_GetValue()) >= 0 && options->entryCount <= 23) {
            options->entries[options->entryCount].kind = kind;
            options->entries[options->entryCount].value = Script_GetValue();
            options->entries[options->entryCount].parameter = Script_GetValue();
            options->entries[options->entryCount].mode = Script_GetValue();
            options->entryCount++;
        }
    }
    if (Script_SeekToKeyword('O'))
        options->option = Script_GetValueSafe();
}
