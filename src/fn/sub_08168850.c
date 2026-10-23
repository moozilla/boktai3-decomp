#include "global.h"

struct ChoiceRecord {
    u8 slot;
    u8 value;
    u8 kind;
    u8 icon;
    u8 *entry;
};
struct ChoiceState {
    u8 pad0[0x50];
    s16 selection;
    u8 pad52[0x68 - 0x52];
    s16 equipment[8];
    s16 unlocks[8];
};

u8 *Inventory_GetEquipmentEntry(s32);
u8 *GameState_GetUnlockDescriptor(s32);

void sub_08168850(u8 *p)
{
    s32 count = 0;
    s32 i;
    struct ChoiceRecord *rec;

    i = 0xf4d;
    *(u8 *)(p + i) = count;
    *(u8 *)(p + 0xf48) = 0xff;
    if (*(*(u8 **)(p + 0xa40) + 0x418) != 6) {
        struct ChoiceState **state;
        u8 *eqc;
        u32 kind0;
        i = 0;
        state = (struct ChoiceState **)0x02000710;
        kind0 = 0;
        rec = (struct ChoiceRecord *)(p + 0xda8);
        eqc = p + 0xf4d;
        for (; i <= 7; i++) {
            u8 *q = (u8 *)*state;
            s16 *slot;
            q += i * 2;
            slot = (s16 *)q;
            slot += 0x34;
            if (*slot >= 0) {
                u8 *e = Inventory_GetEquipmentEntry(*slot);
                struct ChoiceState *gs;
                rec->slot = i;
                gs = *state;
                rec->value = *(u16 *)(i * 2 + (u8 *)gs + 0x68);
                rec->entry = e;
                rec->icon = *e;
                rec->kind = kind0;
                if (i == gs->selection)
                    *(s8 *)(p + 0xf48) = count;
                rec++;
                count++;
                (*eqc)++;
                state = (struct ChoiceState **)0x02000710;
            }
        }
        {
            struct ChoiceState **st2;
            u32 zero;
            i = 0;
            st2 = (struct ChoiceState **)0x02000710;
            zero = 0;
            {
                s32 off = count * 8 + 0xda8;
                rec = (struct ChoiceRecord *)(off + (u32)p);
            }
            for (; i <= 7; i++) {
                u8 *q = (u8 *)*st2;
                s16 *slot;
                {
                    u32 t = i * 2 + (u32)q;
                    slot = (s16 *)t;
                }
                slot += 0x3c;
                if (*slot >= 0) {
                    u8 *e = GameState_GetUnlockDescriptor(*slot);
                    s32 s = i + 8;
                    struct ChoiceState *gs;
                    rec->slot = s;
                    gs = *st2;
                    rec->value = *(u16 *)(i * 2 + (u8 *)gs + 0x78);
                    rec->entry = (u8 *)zero;
                    rec->icon = *e;
                    rec->kind = 1;
                    if (s == gs->selection)
                        *(s8 *)(p + 0xf48) = count;
                    rec++;
                    count++;
                }
            }
        }
        if (*(s8 *)(p + 0xf48) == -1) {
            if ((*(struct ChoiceState **)0x02000710)->selection <= 7)
                *(s8 *)(p + 0xf48) = 0;
            else
                *(s8 *)(p + 0xf48) = *(u8 *)(p + 0xf4d);
        }
    } else {
        struct ChoiceRecord *r0 = (struct ChoiceRecord *)(p + 0xda8);
        r0->slot = 3;
        r0->value = count;
        r0->entry = p + 0x4884;
        *(s8 *)(p + 0xf48) = count;
        count = 1;
    }
    *(u8 *)(p + 0x13f2) = count;
    if (count <= 15) {
        for (; count <= 15; count++) {
            struct ChoiceRecord *r1 = (struct ChoiceRecord *)((count * 8 + 0xda8) + p);
            r1->slot |= 0xff;
            r1->value |= 0xff;
            r1->entry = 0;
        }
    }
}
