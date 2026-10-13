#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Animation {
    u8 pad[6];
    u16 frame;
    u8 tail[20];
};
struct Controller {
    u8 task[24];
    int count;
    struct Animation animations[4];
    u8 indicators[16][180];
    int tile;
};
struct Map {
    u8 pad[36];
    u16 rows[1];
};
extern struct Controller *gUnk_0200004C;
extern u8 *gUnk_02000580;
extern struct Map *gUnk_030052F4;
extern u32 gUnk_030052F8, gUnk_030052FC;
int Script_SeekToKeyword(int);
int Script_GetPc(void);
int Script_GetValue(void);
int sub_082151E4(void *, int);
void sub_08215284(void *, int);
void sub_08011890(void *, void *);
#define SceneIndicators_Initialize sub_08011AD0
int SceneIndicators_Initialize(struct Controller *m)
{
    int i;
    struct Animation *a;
    u8 *first;
    gUnk_0200004C = m;
    m->count = 0;
    if (Script_SeekToKeyword('t')) {
        for (i = 0, a = m->animations; i < 4; a++, i++) {
            int value;
            if (Script_GetPc()) {
                value = Script_GetValue();
                m->count++;
            } else
                value = 0x4211;
            if (!sub_082151E4(a, value))
                return -1;
        }
    } else {
        for (i = 0, a = m->animations; i < 4; a++, i++)
            if (!sub_082151E4(a, 0x4211))
                return -1;
        m->count = 4;
    }
    asm("" : "+r"(m));
    {
        int has = Script_SeekToKeyword('p');
        first = m->indicators[0];
        if (has) {
            for (a = m->animations, i = 3; i >= 0; a++, i--) {
                int value;
                if (Script_GetPc()) {
                    value = Script_GetValue();
                    m->count++;
                } else
                    value = 0;
                sub_08215284(a, a->frame + value);
            }
        }
    }
    {
        u8 *p = first;
        for (i = 15; i >= 0; i--, p += 180)
            sub_08011890(m, p);
    }
    if (gUnk_02000580) {
        int x = *(s16 *)(gUnk_02000580 + 0x30) >> 8, y = *(s16 *)(gUnk_02000580 + 0x34) >> 8, tile;
        if (x < 0 || y < 0 || x >= gUnk_030052F8 || y >= gUnk_030052FC)
            tile = 0;
        else
            tile = gUnk_030052F4->rows[y] + x;
        m->tile = tile;
    }
    return 0;
}
