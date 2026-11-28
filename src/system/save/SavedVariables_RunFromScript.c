#include "global.h"
#include "system/save/saved_variables.h"
void *Heap_Alloc(s32 size);
void Memory_Clear(void *dst, s32 bytes);
s32 Heap_Free(void *block); /* void in engine.h; the caller returns its r0 */
s32 SavedVariables_CaptureVariableValues(struct SavedVariables *);
void GameState_ResetActiveRegions(void);
s32 SavedVariables_RestoreVariableValues(struct SavedVariables *);
/* Registry routine 3C8D: runs SavedVariables_CaptureVariableValues/SavedVariables_RestoreVariableValues on a cleared
   0x1804-byte heap buffer, then frees it; -1 on failure (tentative). */
s32 SavedVariables_RunFromScript(void)
{
    struct SavedVariables *save = Heap_Alloc(sizeof(struct SavedVariables));
    if (save == NULL)
        return -1;
    Memory_Clear(save, sizeof(struct SavedVariables));
    if (SavedVariables_CaptureVariableValues(save) < 0)
        return -1;
    GameState_ResetActiveRegions();
    if (SavedVariables_RestoreVariableValues(save) >= 0)
        return Heap_Free(save);
    return -1;
}
