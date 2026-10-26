#include "global.h"
#include "scene_item_values.h"
#include "ram.h"
extern u8 gUnk_08E6092C[][0x20];
extern u8 gUnk_08E60F80[][16];
extern u16 gUnk_086141BC[];
void sub_0801B600(u32 a, u32 b, struct SceneValueEquipment *c);
void TransformMarker_CopyVector(u8 *p, u8 *q);
void TransformMarker_ClearVector(u8 *p);
void Inventory_MarkEquipmentObtained(u32);
void GameState_ClearConsumableSlot(u32);
s32 Div(s32, s32);

struct TfCtx {
    u8 pad0[0x7c];
    struct SceneValueEquipment cur;
    u8 pad1[0xa0 - 0x8c];
    u16 a0, a2, a4, a6;
    u8 pad2[0xba - 0xa8];
    u8 b8a, b8b, b8c;
    u8 pad3[0xc5 - 0xbd];
    u8 c5, c6;
    u8 pad4;
    u16 c8;
};
void sub_0801B700(struct TfCtx *p)
{
    struct SceneValueEquipment *e = Inventory_GetEquipmentEntry(p->b8a);
    u8 *t2 = gUnk_08E60F80[Inventory_GetConsumableId(p->b8b)];
    u8 *t1;
    u8 *t3;
    s32 th;
    s32 a, b, v;
    u32 k;
    u8 flag;

    sub_0801B600(p->b8a, p->b8b, &p->cur);
    t1 = gUnk_08E6092C[e->kind];
    t3 = gUnk_08E6092C[p->cur.kind];
    p->c8 = t3[2] - t1[2];
    flag = 0;
    if (p->cur.grade > e->grade)
        flag = 1;
    p->c6 = flag;
    switch (t2[0xa]) {
        case 20:
            th = 0x1999;
            break;
        case 30:
            th = 0x2666;
            break;
        case 40:
            th = 0x3333;
            break;
        case 50:
            th = 0x4000;
            break;
        case 60:
            th = 0x8000;
            break;
        case 0xff:
            th = 0x10000;
            break;
        case 10:
        default:
            th = 0xccc;
            break;
    }
    if (p->b8c)
        th <<= 1;
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 0x3ff;
    if (gUnk_086141BC[gEffectRandomTableIndex] < th) {
        TransformMarker_CopyVector((u8 *)&p->cur, t3);
    } else {
        if (e->kind != p->cur.kind)
            TransformMarker_ClearVector((u8 *)&p->cur);
    }
    if (t3[0xc] == 0 && t3[0x10] == 0)
        p->c5 = 0;
    else if (e->kind == p->cur.kind) {
        if (e->modifierA || e->modifierB)
            p->c5 = 2;
        else if (p->cur.modifierA || p->cur.modifierB)
            p->c5 = 3;
        else
            p->c5 = 1;
    } else if (p->cur.modifierA || p->cur.modifierB)
        p->c5 = 3;
    else
        p->c5 = 1;
    a = Div(p->a0 << 8, p->a2);
    b = Div(p->a4 << 8, p->a6);
    v = (a * 0x66 + b * 0x99) >> 8;
    if (v > 0x400)
        p->cur.condition = 1000;
    else if (v > 0x300)
        p->cur.condition = 750;
    else if (v > 0x200)
        p->cur.condition = 500;
    else
        p->cur.condition = 250;
    *e = p->cur;
    Inventory_MarkEquipmentObtained(p->cur.kind);
    k = p->cur.kind;
    if ((s32)k < 0x20)
        *(u32 *)((u8 *)gGameState + 0x748) |= 1 << k;
    else
        *(u32 *)((u8 *)gGameState + 0x74c) |= 1 << (k - 0x20);
    GameState_ClearConsumableSlot(p->b8b);
}
