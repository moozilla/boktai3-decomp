#include "global.h"
struct Sprite {
    u8 pad[16];
    u16 frame;
};
struct Indicator {
    u8 pad0[8];
    u32 timer;
    u8 padc[5];
    s8 frame;
    u8 pad12, index, count, visible, mode, disabled;
    int delay, period, value;
    s16 times[8];
    u8 effect[84];
    struct Sprite sprite;
};
void Sound_PlaySongChecked(int);
void Collider_Refresh(void *);
static inline void setFrame(struct Sprite *s, int frame)
{
    s->frame = frame;
}
void *sub_08011884(void);
struct Indicator *sub_08011C6C(void *, int);
int Script_GetKeywordValue(int, int);
int SceneIndicator_Resume(void)
{
    void *m = sub_08011884();
    int id;
    struct Indicator *p;
    if (!m)
        return -1;
    id = Script_GetKeywordValue('n', 0);
    if (!id)
        return -1;
    p = sub_08011C6C(m, id);
    if (!p || !p->disabled)
        return -1;
    p->disabled = 0;
    switch (p->mode) {
        case 0:
            p->index = 0;
            break;
        case 1:
            p->delay = 0;
            break;
        case 2:
            p->visible = 1;
            setFrame(&p->sprite, p->frame + 1);
            break;
    }
    return 0;
}
