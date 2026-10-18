#include "global.h"
struct CommandSet {
    void *next;
    u32 count;
    const void *entries;
};
extern struct CommandSet gUnk_030025E8;
extern const u8 gUnk_08E8791C[];
void Script_InitEngineCommandHook(void);
void Script_ClearCommandTables(void);
u32 Script_RegisterCommandTable(struct CommandSet *);
u32 Script_RegisterEngineCommands(void)
{
    Script_InitEngineCommandHook();
    Script_ClearCommandTables();
    gUnk_030025E8.next = 0;
    gUnk_030025E8.count = 8;
    gUnk_030025E8.entries = gUnk_08E8791C;
    return Script_RegisterCommandTable(&gUnk_030025E8);
}
