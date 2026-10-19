#include "global.h"
#include "modifier_list.h"
#define Modifier_FindSourceRecord sub_0811E37C
#define gModifierSources gUnk_02000208
extern struct ModifierSourceList *gUnk_02000208;
s32 sub_0811E37C(struct ModifierRecord *out, s32 id)
{
    struct ModifierSourceList *list;
    s32 i, count;
    if (gUnk_02000208) {
        goto search;
found:
        out->order = list->id;
        out->kind = list->kind;
        out->amount = list->amount;
        out->extra = list->extra;
        return i;
search:
        list = gUnk_02000208;
        i = 0;
        if (i < list->count) {
            count = list->count;
            do {
                if ((s8)list->id == id) goto found;
                list = (struct ModifierSourceList *)((u8 *)list + 8);
                i++;
            } while (i < count);
        }
    }
    return -1;
}
