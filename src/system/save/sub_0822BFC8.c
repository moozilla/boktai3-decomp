#include "global.h"
void *Heap_Alloc(s32 size);
void Memory_Clear(void *dst, s32 bytes);
s32 Heap_Free(void *block); /* void in engine.h; the caller returns its r0 */
s32 SavedVariables_CaptureVariableValues(void *);
void GameState_ResetActiveRegions(void);
s32 SavedVariables_RestoreVariableValues(void *);
/* Registry routine 3C8D: runs SavedVariables_CaptureVariableValues/SavedVariables_RestoreVariableValues on a cleared
   0x1804-byte heap buffer, then frees it; -1 on failure (tentative). */
s32 SavedVariables_RunFromScript(void)
{
    void *p = Heap_Alloc(0x1804);
    if (p == NULL)
        return -1;
    Memory_Clear(p, 0x1804);
    if (SavedVariables_CaptureVariableValues(p) < 0)
        return -1;
    GameState_ResetActiveRegions();
    if (SavedVariables_RestoreVariableValues(p) < 0)
        goto fail;
    return Heap_Free(p);
fail:
    return -1;
}
