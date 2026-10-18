#define ScriptRecord_ReadEntries sub_0811E224
#include "global.h"
struct ScriptRecordEntry {
    u8 kind, a, b, c;
    u16 value;
    u8 pad[2];
};
struct ScriptRecord {
    u8 pad[0x1f8];
    u8 *entriesPc;
    s32 option;
    struct ScriptRecordEntry entries[6];
    s32 count;
};
void Memory_Clear(void *, u32);
s32 Script_GetValue(void);
s32 Script_GetValueSafe(void);
s32 Script_SeekToKeyword(u32);
void Script_SetPc(u8 *);
void sub_0811E224(struct ScriptRecord *p)
{
    s32 kind;
    if (Script_SeekToKeyword('b')) {
        p->entriesPc = (u8 *)Script_GetValueSafe();
        Memory_Clear(p->entries, 0x30);
        Script_SetPc(p->entriesPc);
        p->count = 0;
        while ((kind = Script_GetValue()) >= 0 && p->count <= 5) {
            p->entries[p->count].kind = kind;
            p->entries[p->count].a = Script_GetValue();
            p->entries[p->count].b = Script_GetValue();
            p->entries[p->count].c = Script_GetValue();
            p->entries[p->count].value = Script_GetValue();
            p->count++;
        }
    }
    if (Script_SeekToKeyword('B'))
        p->option = Script_GetValueSafe();
}
