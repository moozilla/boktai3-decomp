#include "global.h"
#include "scene_item_values.h"
extern u8 gUnk_08E6092C[][0x20];
extern u8 gUnk_08E60F80[][16];
extern u8 gUnk_0824DB28[];
void TransformMarker_SetTypeFromTable(u8 *p, u8 *q, u32 i);
void TransformMarker_ClearVector(u8 *p);

void sub_0801B600(u32 a, u32 b, struct SceneValueEquipment *c)
{
    struct SceneValueEquipment *e = Inventory_GetEquipmentEntry(a);
    s32 idx = Inventory_GetConsumableId(b);
    u8 *t1 = gUnk_08E6092C[e->kind];
    u8 *t2 = gUnk_08E60F80[idx];
    u8 *e2;
    u32 x;

    *c = *e;
    if (c->condition == 0) {
        u32 want = t2[0xb];
        x = t1[0x1e];
        if (x != 0xff) {
            e2 = gUnk_08E6092C[x];
            if (e2[1] == want && e2[2] <= t2[0xa])
                goto use;
        }
        if (t1[1] != want) {
            TransformMarker_SetTypeFromTable((u8 *)c, t1, 0);
            TransformMarker_ClearVector((u8 *)c);
        }
        return;
    } else {
        u32 tmp;
        u32 lim;
        u32 want;
        u32 i;
        u8 *s;
        u8 *tbl;
        if (t1[2] > (tmp = t2[0xa])) {
            if (t1[1] != t2[0xb]) {
                x = t1[0x1c];
                if (x != 0xff) {
                    t1 = gUnk_08E6092C[x];
                    c->kind = t1[0];
                    TransformMarker_SetTypeFromTable((u8 *)c, t1, 0);
                }
            }
            return;
        }
        goto loop;
    use:
        t1 = e2;
        c->kind = t1[0];
        TransformMarker_SetTypeFromTable((u8 *)c, t1, 0);
        return;
    loop:
        want = t2[0xb];
        i = 0;
        tbl = gUnk_0824DB28;
        lim = tmp;
        s = t1 + 0x18;
        for (; i <= 4; i++) {
            x = s[*(u8 *)(i + (u32)tbl)];
            if (x != 0xff) {
                e2 = gUnk_08E6092C[x];
                if (e2[1] == want && e2[2] <= lim)
                    goto use;
            }
        }
        {
            s32 n = c->grade;
            if (n <= 1)
                n++;
            TransformMarker_SetTypeFromTable((u8 *)c, t1, n);
        }
        return;
    }
}
