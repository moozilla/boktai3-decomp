#include "global.h"
#include "system/save/saved_variables.h"
/* Write saved values back through the references read by SavedVariables_CaptureVariableValues. */
s32 SavedVariables_RestoreVariableValues(struct SavedVariables *save)
{
    struct ScriptUtilityReference *reference = save->references;
    u32 *value = save->values;
    u32 restored = 0;

    while (restored < save->count) {
        s32 i;

        for (i = 0; i < reference->count && restored < save->count; i++) {
            Script_WriteReferenceValue(reference, i, *value++);
            restored++;
        }
        reference++;
    }
    return 0;
}
