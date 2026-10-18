#include "global.h"
void ScriptOptions_LoadEntries(void *);
void ScriptOptions_LoadTargets(void *);
void ScriptOptions_LoadWideRecords(void *);
void ScriptOptions_LoadShortRecords(void *);
void ScriptRecord_ReadEntries(void *);
void Modifier_LoadTablesFromScript(void *p)
{
    ScriptOptions_LoadEntries(p);
    ScriptOptions_LoadTargets(p);
    ScriptOptions_LoadWideRecords(p);
    ScriptOptions_LoadShortRecords(p);
    ScriptRecord_ReadEntries(p);
}
