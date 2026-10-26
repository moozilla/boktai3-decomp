#include "global.h"
#include "ram.h"
struct ScriptAttributeModifier {
    s8 id;
    u8 kind;
    u16 a, b, c, d, pad;
};
struct ScriptRecordTable {
    u8 pad[0x1ac];
    struct ScriptAttributeModifier entries[6];
    s32 count;
};
struct AttributeModifierResult {
    s8 id;
    u8 kind;
    u16 a, b, c, d;
};
s32 ScriptRecord_FindAttributeModifier(struct AttributeModifierResult *out, s32 id)
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
