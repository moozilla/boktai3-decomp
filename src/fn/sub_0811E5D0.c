#define gScriptRecordTable gUnk_02000208
#include "global.h"
struct ScriptByteModifier {
    s8 id;
    u8 kind;
    u8 a, b;
    u16 value;
    u8 pad[2];
};
struct ScriptRecordTable {
    u8 pad[0x200];
    struct ScriptByteModifier entries[6];
    s32 count;
};
struct ByteModifierResult {
    s8 id;
    u8 kind;
    u8 a, b;
    u16 value;
};
extern struct ScriptRecordTable *gUnk_02000208;
s32 ScriptRecord_FindByteModifier(struct ByteModifierResult *out, s32 id)
{
    struct ScriptRecordTable *p;
    struct ScriptRecordTable *table = gUnk_02000208;
    s32 i;
    if (table)
        goto search;
    goto missing;
found:
    out->id = p->entries[i].id;
    out->kind = p->entries[i].kind;
    out->a = p->entries[i].a;
    out->b = p->entries[i].b;
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
