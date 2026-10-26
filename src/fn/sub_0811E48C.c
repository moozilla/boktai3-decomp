#include "global.h"
#include "ram.h"
struct ScriptExtendedModifier {
    s8 id;
    u8 kind;
    u16 a, b, c, d, e, f, pad;
};
struct ScriptRecordTable {
    u8 pad[0x140];
    struct ScriptExtendedModifier entries[6];
    s32 count;
};
struct ExtendedModifierResult {
    s8 id;
    u8 kind;
    u16 a, b, c, d, e, f;
};
s32 ScriptRecord_FindExtendedModifier(struct ExtendedModifierResult *out, s32 id)
{
    struct ScriptRecordTable *p;
    struct ScriptRecordTable *table = gScriptRecordTable;
    s32 i;
    if (table)
        goto search;
    goto missing;
found:
    out->id = p->entries[i].id;
    out->kind = p->entries[i].kind;
    out->a = p->entries[i].a;
    out->b = p->entries[i].b;
    out->c = p->entries[i].c;
    out->d = p->entries[i].d;
    out->f = p->entries[i].f;
    out->e = 0;
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
