#include "global.h"

typedef struct {
    u16 id;
    u8 unk[4];
    u8 count[2];
    s16 *messages[2][4];
    u8 pad[8];
    void *next;
} Entry;

struct Vec {
    s16 x, y, z, w;
};

extern u8 *gUnk_0200047C;

void sub_08020D54(void *, void *);
void sub_0814F858(u8 *);
void sub_0814F874(u8 *);
void sub_0815AEE0(u8 *);
void sub_0815AED4(u8 *, struct Vec *);
void sub_0815AF14(u8 *, s32);
void sub_0815AF50(u8 *, s32, s32);
void sub_0815AF8C(u8 *, s32);
void sub_0815AFC8(u8 *, s32, s32);
void sub_0815B010(u8 *, s32, s32);
void sub_0815B0A8(u8 *, struct Vec *, s32, s32);
void sub_0815B1D0(u8 *, s32, s32);
void sub_0815B238(u8 *, s32, s32);
void sub_0815B28C(u8 *, s32);
void sub_0815B2E0(u8 *, s32, s32);
void sub_0815B378(u8 *, s32);
void sub_0815B330(u8 *, s32);
void sub_0815B3BC(u8 *, s32, s32);
void sub_0815B15C(u8 *, struct Vec *, s32, s32, s32);
void sub_0815B52C(u8 *, s32, s32, s32);
void sub_0815B5F8(u8 *, s32, s32, s32, s32);
void sub_0815B69C(u8 *, s32);
void sub_0815B6DC(u8 *, s32, s32, s32);
void sub_0815B858(u8 *, s32, s32);
void sub_0815B9CC(u8 *, s32, s32);
void sub_0815B968(u8 *, s32, s32);
void sub_0815BC74(u8 *, s32, s32);
void sub_0815BCBC(u8 *, s32, s32);
void sub_0815BDD4(u8 *, s32, s32, s32);
void sub_0815BE28(u8 *, s32);
void sub_0815C3B8(u8 *, s32);
s32 sub_0815C3F0(u8 *, s32);
s32 sub_0815C428(u8 *, s32);
s32 sub_0815C498(u8 *, struct Vec *, s32);
s32 sub_0815C4FC(u8 *, s32);
void sub_0815BF18(u8 *, struct Vec *, s32, s32);
void sub_0815BF60(u8 *, s32);
void sub_0815C040(u8 *, s32, s32);
void sub_0815C0BC(u8 *, s32, s32);
void sub_0815C138(u8 *, s32, s32);
void sub_0815C1B4(u8 *, s32, s32);
void sub_0815C230(u8 *, s32, s32);
void sub_0815C2BC(u8 *, s32, s32);
void sub_0815C348(u8 *, s32);
void sub_0815BFB8(u8 *, s32);
void sub_0815BFFC(u8 *, s32);
void sub_0815C5A4(u8 *, s32);
void sub_0815C730(u8 *);
void sub_0815AE94(u8 *, s32);
void sub_0815AEAC(u8 *, s32);
void sub_0815AEC4(u8 *);
void sub_0815C73C(u8 *, s32, s32, s32);

void sub_0815D6B0(u8 *p)
{
    Entry *entry = (Entry *)(p + 0x2F8);
    s32 i;
    struct Vec v1, v7, v51, v33;

    i = 0;
    if (i < entry->count[gUnk_0200047C[0x24]])
        do {
            s16 *m;
            u8 *controller = gUnk_0200047C;
            m = entry->messages[controller[0x24]][i];
            sub_08020D54(entry, m);
            switch (((u8 *)m)[6]) {
                case 0:
                    sub_0815AEE0(p);
                    sub_0814F858(p);
                    sub_0814F874(p);
                    break;
                case 1:
                    v1.x = m[4];
                    v1.y = m[5];
                    v1.z = m[6];
                    sub_0815AED4(p, &v1);
                    sub_0814F858(p);
                    sub_0814F874(p);
                    break;
                case 2:
                    sub_0815AF14(p, m[4]);
                    sub_0814F858(p);
                    sub_0814F874(p);
                    break;
                case 3:
                    sub_0815AF50(p, m[4], 0);
                    sub_0814F858(p);
                    break;
                case 4:
                    sub_0815AF8C(p, m[4]);
                    sub_0814F858(p);
                    sub_0814F874(p);
                    break;
                case 5:
                    sub_0815AFC8(p, m[4], 0);
                    sub_0814F858(p);
                    break;
                case 6:
                    sub_0815B010(p, m[4], 0);
                    sub_0814F858(p);
                    break;
                case 7:
                    v7.x = m[4];
                    v7.y = m[5];
                    v7.z = m[6];
                    sub_0815B0A8(p, &v7, 0, -1);
                    sub_0814F858(p);
                    break;
                case 8:
                    sub_0815B1D0(p, m[4], 0);
                    sub_0814F858(p);
                    break;
                case 9:
                    sub_0815B238(p, m[4], 0);
                    sub_0814F858(p);
                    break;
                case 10:
                    sub_0815B28C(p, 0);
                    sub_0814F858(p);
                    break;
                case 11:
                    sub_0815B2E0(p, m[4], 0);
                    sub_0814F858(p);
                    break;
                case 12:
                    sub_0815B378(p, m[4]);
                    sub_0814F858(p);
                    sub_0814F874(p);
                    break;
                case 13:
                    sub_0815B330(p, m[4]);
                    sub_0814F858(p);
                    sub_0814F874(p);
                    break;
                case 14:
                    sub_0815B3BC(p, m[4], 0);
                    sub_0814F858(p);
                    break;
                case 15:
                    sub_0815B15C(p, 0, m[4], 0, 0);
                    sub_0814F858(p);
                    sub_0814F874(p);
                    break;
                case 16:
                    sub_0815B52C(p, m[4], m[5], 0);
                    sub_0814F858(p);
                    break;
                case 17:
                    sub_0815B5F8(p, m[4], m[5], m[6], 0);
                    sub_0814F858(p);
                    break;
                case 18:
                    sub_0815B69C(p, 0);
                    sub_0814F858(p);
                    break;
                case 19:
                    sub_0815B6DC(p, m[4], m[5], 0);
                    sub_0814F858(p);
                    break;
                case 20:
                    sub_0815B858(p, m[4], 0);
                    sub_0814F858(p);
                    break;
                case 22:
                    sub_0815B9CC(p, m[4], 0);
                    sub_0814F858(p);
                    break;
                case 21:
                    sub_0815B968(p, m[4], 0);
                    sub_0814F858(p);
                    break;
                case 23:
                    sub_0815BC74(p, m[4], 0);
                    sub_0814F858(p);
                    break;
                case 24:
                    sub_0815BCBC(p, m[4], 0);
                    sub_0814F858(p);
                    break;
                case 25:
                    sub_0815BDD4(p, m[4], m[5], 0);
                    sub_0814F858(p);
                    break;
                case 26:
                    sub_0815BE28(p, 0);
                    sub_0814F858(p);
                    break;
                case 28:
                    sub_0815C3B8(p, 0);
                    sub_0814F858(p);
                    break;
                case 29: {
                    s32 r = sub_0815C3F0(p, 0);
                    sub_0814F858(p);
                    if (r == 0)
                        sub_0814F874(p);
                    break;
                }
                case 30: {
                    s32 r = sub_0815C428(p, 0);
                    sub_0814F858(p);
                    if (r == 0)
                        sub_0814F874(p);
                    break;
                }
                case 51:
                    v51.x = m[4];
                    v51.y = m[5];
                    v51.z = m[6];
                    sub_0815C498(p, &v51, 0);
                    break;
                case 31: {
                    s32 r = sub_0815C4FC(p, 0);
                    sub_0814F858(p);
                    if (r == 0)
                        sub_0814F874(p);
                    break;
                }
                case 33:
                    v33.x = m[4];
                    v33.y = m[5];
                    v33.z = m[6];
                    sub_0815BF18(p, &v33, -1, 0);
                    sub_0814F858(p);
                    break;
                case 34:
                    sub_0815BF60(p, m[4]);
                    sub_0814F858(p);
                    sub_0814F874(p);
                    break;
                case 35:
                    sub_0815C040(p, m[4], 0);
                    sub_0814F858(p);
                    break;
                case 37:
                    sub_0815C0BC(p, m[4], 0);
                    sub_0814F858(p);
                    break;
                case 38:
                    sub_0815C138(p, m[4], 0);
                    sub_0814F858(p);
                    break;
                case 36:
                    sub_0815C1B4(p, m[4], 0);
                    sub_0814F858(p);
                    break;
                case 39:
                    sub_0815C230(p, m[4], 0);
                    sub_0814F858(p);
                    break;
                case 40:
                    sub_0815C2BC(p, m[4], 0);
                    sub_0814F858(p);
                    break;
                case 27:
                    sub_0815C348(p, m[4]);
                    sub_0814F858(p);
                    sub_0814F874(p);
                    break;
                case 41:
                    sub_0815BFB8(p, 0);
                    sub_0814F858(p);
                    break;
                case 42:
                    sub_0815BFFC(p, 0);
                    sub_0814F858(p);
                    break;
                case 43:
                    sub_0815C5A4(p, 2);
                    sub_0814F858(p);
                    sub_0814F874(p);
                    break;
                case 44:
                    sub_0815C730(p);
                    sub_0814F858(p);
                    sub_0814F874(p);
                    break;
                case 48:
                    sub_0815AE94(p, m[4] + 0x16E);
                    sub_0814F858(p);
                    sub_0814F874(p);
                    break;
                case 47:
                    sub_0815AEAC(p, m[4] + 0x16E);
                    sub_0814F858(p);
                    sub_0814F874(p);
                    break;
                case 49:
                    sub_0815AEC4(p);
                    sub_0814F858(p);
                    sub_0814F874(p);
                    break;
                case 50:
                    sub_0815AE94(p, 0x17A);
                    sub_0814F858(p);
                    sub_0814F874(p);
                    break;
                case 45:
                    p[0x5A1] = 1;
                    sub_0814F858(p);
                    sub_0814F874(p);
                    break;
                case 46:
                    p[0x5A1] = 0;
                    sub_0814F858(p);
                    sub_0814F874(p);
                    break;
                case 32:
                    switch (m[4]) {
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                        case 6:
                        case 7:
                        case 8:
                            sub_0815C73C(p, m[4], 0, 0);
                            break;
                        case 4:
                        case 5:
                            sub_0815C73C(p, m[4], m[5], m[6]);
                            break;
                    }
                    sub_0814F858(p);
                    sub_0814F874(p);
                    break;
            }
            i++;
        } while (i < entry->count[gUnk_0200047C[0x24]]);
}
