#include "global.h"
#include "system/save/saved_variables.h"
u8 *Script_GetPc(void);
u32 Script_ReadReferenceValue(struct ScriptUtilityReference *, s32);
/* Read variable references from the script and save up to 512 of their values. */
s32 SavedVariables_CaptureVariableValues(struct SavedVariables *save)
{
    struct ScriptUtilityReference *reference = save->references;
    u32 *value = save->values;

    while (save->count < 512 && Script_GetPc()) {
        s32 i;

        Script_ReadReference(reference);
        for (i = 0; i < reference->count && save->count < 512; i++) {
            *value++ = Script_ReadReferenceValue(reference, i);
            save->count++;
        }
        reference++;
    }
    return 0;
}
