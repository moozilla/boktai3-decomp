#include "global.h"
#include "ram.h"
struct ScriptModifier {
    s8 id;
    u8 pad;
    u16 a, b, c;
    u32 value;
};
struct ScriptRecordTable {
    u8 pad[0xec];
    struct ScriptModifier entries[6];
    s32 count;
};
struct ModifierResult {
    s8 id;
    u8 pad;
    u16 a, b, c;
    u32 value;
};
s32 ScriptRecord_FindModifier(struct ModifierResult *out, s32 id)
{
    struct ScriptRecordTable *p;
    struct ScriptRecordTable *table = gScriptRecordTable;
    s32 i, count;
    if (table)
        goto search;
    goto missing;
found:
    out->id = p->entries[i].id;
    out->a = p->entries[i].a;
    out->b = p->entries[i].b;
    out->c = p->entries[i].c;
    out->value = p->entries[i].value;
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
