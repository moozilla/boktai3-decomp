#include "global.h"
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
int AnimData_Load(void *, int);
void AnimData_SelectTiles(void *, int);
void SceneIndicator_Initialize(void *, void *);
void SceneIndicator_Update(void *, void *);
int SceneIndicators_Update(struct Controller *m)
{
    int i;
    u8 *p = m->indicators[0];
    for (i = 15; i >= 0; i--, p += 180)
        if (p[16])
            SceneIndicator_Update(m, p);
    {
        int x = *(s16 *)(gUnk_02000580 + 0x30) >> 8, y = *(s16 *)(gUnk_02000580 + 0x34) >> 8, tile;
        if (x < 0 || y < 0 || x >= gUnk_030052F8 || y >= gUnk_030052FC)
            tile = 0;
        else
            tile = gUnk_030052F4->rows[y] + x;
        m->tile = tile;
    }
    return 0;
}
