#include "global.h"
#include "ram.h"
struct CommandSet {
    void *next;
    u32 count;
    const void *entries;
};
extern const u8 gUnk_08E8791C[];
void Script_InitEngineCommandHook(void);
void Script_ClearCommandTables(void);
u32 Script_RegisterCommandTable(struct CommandSet *);
u32 Script_RegisterEngineCommands(void)
{
    Script_InitEngineCommandHook();
    Script_ClearCommandTables();
    gCommandSet.next = 0;
    gCommandSet.count = 8;
    gCommandSet.entries = gUnk_08E8791C;
    return Script_RegisterCommandTable(&gCommandSet);
}
