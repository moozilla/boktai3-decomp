#include "global.h"
s32 Inventory_GetEquipmentId(s32);
void sub_0822D94C(s32, s32);
#define Inventory_SortEquipment sub_0822DA94
void Inventory_SortEquipment(s32 base)
{
    s32 n = 0;
    s32 gaps = 0;
    s32 i;
    s32 j;
    s32 a;
    for (i = 0; i <= 15; i++) {
        s32 idx = base + i;
        if (Inventory_GetEquipmentId(idx) == 0xFF) {
            gaps++;
        } else {
            n++;
            if (gaps > 0)
                sub_0822D94C(idx, idx - gaps);
        }
    }
    if (n > 1) {
        s32 m;
        i = 0;
        m = n - 1;
        for (; i < m; i++) {
            for (j = i; j < n; j++) {
                a = Inventory_GetEquipmentId(base + i);
                if (a > Inventory_GetEquipmentId(base + j))
                    sub_0822D94C(base + i, base + j);
            }
        }
    }
}
