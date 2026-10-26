#include "global.h"
#include "ram.h"
struct ScriptModifier {
    s8 id;
    u8 kind;
    u16 amount;
    u8 extra;
    u8 pad[3];
};
struct ScriptModifierTable {
    u8 pad[0x20];
    struct ScriptModifier entries[24];
    s32 count;
};
struct ModifierOut {
    s8 id;
    u8 kind;
    u16 amount;
    u8 extra;
};
s32 ScriptRecord_FindModifierRecord(struct ModifierOut *out, s32 id)
{
    struct ScriptModifierTable *p;
    struct ScriptModifierTable *table = gScriptRecordTable;
    s32 i;
    if (table)
        goto search;
    goto missing;
found:
    out->id = p->entries[i].id;
    out->kind = p->entries[i].kind;
    out->amount = p->entries[i].amount;
    out->extra = p->entries[i].extra;
    return i;
search:
    p = table;
    for (i = 0; i < p->count; i++) {
        if (p->entries[i].id == id)
            goto found;
    }
missing:
    return -1;
}
