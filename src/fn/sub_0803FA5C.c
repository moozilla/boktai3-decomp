// MATCHING_CONCESSION: register-pinning
#include "modifier_list.h"
#define Modifier_ApplyToStat sub_0803FA5C
s32 sub_0803F95C(struct ModifierActor *, struct ModifierSelection *, struct ModifierRecord *, u32);
s32 sub_0803F934(s32, u32, u32);
s32 Modifier_ApplyToStat(struct ModifierActor *actorArg, struct ModifierSelection *selectionArg,
                         s32 valueArg, u32 includeSelected, u32 kindArg)
{
    register struct ModifierActor *actor asm("r12") = actorArg;
    register struct ModifierSelection *selection asm("r8") = selectionArg;
    u32 kind;
    s32 value;
    /* Original frame reserves an unused eight-byte prefix before the records. */
    struct {
        u8 unused[8];
        struct ModifierRecord records[8];
    } workspace;
    struct ModifierRecord *record;
    register struct ModifierRecord *iterator asm("r2");
    register s32 i asm("r5");
    s32 result;
    register u32 mask asm("r1");
    kind = kindArg;
    value = valueArg;
    record = workspace.records;
    mask = 255;
    asm("" : : "r"(mask));
    iterator = record;
    i = 7;
    do {
        iterator->order = -1;
        iterator++;
    } while (--i >= 0);
    result = sub_0803F95C(actor, selection, record, includeSelected);
    if (result > 0) {
        s32 count = result;
        do {
            if (record->kind == kind)
                value = sub_0803F934(value, kind, record->amount);
            record++;
        } while (--count != 0);
    }
    if (value > 999)
        value = 999;
    return value;
}
