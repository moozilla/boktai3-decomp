#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {
    s16 x, y, z, pad;
};
struct Sprite {
    u32 flags;
    u8 pad[12];
    u16 frame;
    u8 pad12[10];
    struct Vec position;
    u8 tail[8];
};
struct Indicator {
    u8 pad0[8];
    u32 timer;
    int id;
    u8 active;
    s8 frame;
    u8 animation, index, count, visible, mode, disabled;
    int delay, period, value;
    u16 times[8];
    u8 effect[84];
    struct Sprite sprite;
};
void sub_0822B3BC(int);
void sub_0821FE40(void *);
static inline void setFrame(struct Sprite *s, int frame)
{
    s->frame = frame;
}
void *sub_08011884(void);
struct Indicator *sub_08011C6C(void *, int);
int sub_0821ABA8(int, int);
struct Map {
    u32 pad;
    u8 *cells;
    u8 pad8[28];
    u16 rows[1];
};
extern struct Map *gUnk_030052F4;
extern u32 gUnk_030052F8, gUnk_030052FC;
void *sub_08011C4C(void *);
int Script_SeekToKeyword(int);
int Script_GetValue(void);
int Script_GetPc(void);
u8 *sub_0821D64C(int, int);
int sub_0821DA14(void *);
void sub_08214588(void *, void *);
void sub_0821FF24(void *, void *, int);
void sub_0821FF50(void *, int, int);
static inline int cellIndex(struct Vec *p)
{
    int x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline u8 *cell(int i)
{
    u8 *p = sub_0821D64C(i, 1);
    if (p)
        return p + 4;
    return gUnk_030052F4->cells + 12 + i * 4;
}
int SceneIndicator_CreateFromScript(void)
{
    u8 *m = sub_08011884();
    struct Indicator *p;
    struct Vec *position;
    struct Sprite *sprite;
    void *effect;
    if (!m)
        return -1;
    p = sub_08011C4C(m);
    if (!p)
        return -1;
    p->id = sub_0821ABA8('n', 0);
    if (Script_SeekToKeyword('p')) {
        p->sprite.position.x = Script_GetValue();
        p->sprite.position.y = Script_GetValue();
        p->sprite.position.z = Script_GetValue();
    }
    if (sub_0821ABA8('f', 1)) {
        u8 *c;
        int height, slope;
        p->sprite.position.x = (p->sprite.position.x & ~255) + 128;
        p->sprite.position.z = (p->sprite.position.z & ~255) + 128;
        position = &p->sprite.position;
        c = cell(cellIndex(position));
        slope = *c >> 4;
        height = (*c & 15) << 8;
        switch (slope) {
            case 1:
                height -= (u8)position->z;
                break;
            case 2:
                height -= (u8)position->x;
                break;
        }
        p->sprite.position.y = height;
    }
    p->frame = sub_0821ABA8('P', -1);
    if (p->frame < 0) {
        switch (sub_0821DA14(&p->sprite.position) & 14) {
            case 2:
                p->frame = 4;
                break;
            case 6:
                p->frame = 4;
                break;
            case 12:
                p->frame = 2;
                break;
            case 8:
                p->frame = 2;
                break;
            case 0:
            case 4:
            case 10:
            case 14:
            default:
                p->frame = 0;
                break;
        }
    }
    p->index = 0;
    p->count = 0;
    if (Script_SeekToKeyword('t')) {
        register u16 *base asm("r6") = p->times;
        u16 *t = base;
        do {
            if (Script_GetPc()) {
                *t = Script_GetValue();
                p->count++;
            } else {
                register u32 all asm("r0") = 65535;
                *t = all;
            }
            t++;
        } while ((s32)t <= (s32)(base + 7));
        if (p->count & 1)
            return -1;
        p->mode = 0;
        position = &p->sprite.position;
        sprite = &p->sprite;
        effect = p->effect;
    } else {
        int i;
        register u32 mask asm("r3");
        position = &p->sprite.position;
        sprite = &p->sprite;
        effect = p->effect;
        {
            register u32 all asm("r0") = 65535;
            asm("" : : "r"(all));
            mask = all;
        }
        for (i = 0; i < 8; i++)
            p->times[i] |= mask;
    }
    p->delay = 0;
    p->period = sub_0821ABA8('T', -1);
    if (p->period >= 0)
        p->mode = 1;
    if (Script_SeekToKeyword('s')) {
        p->mode = 2;
        p->visible = Script_GetValue();
    }
    p->animation = sub_0821ABA8('E', 0);
    sub_08214588(sprite, m + 28 + p->animation * 28);
    sprite->frame = p->frame + p->visible;
    sub_0821FF24(effect, position, 0);
    if (sub_0821ABA8('v', 1))
        sprite->flags &= ~1;
    else
        sprite->flags |= 1;
    sub_0821FF50(effect, (u16)sub_0821ABA8('d', 10), 1);
    {
        register u32 raw asm("r0") = (u16)position->x;
        register int x asm("r2");
        int z;
        x = (s16)raw >> 8;
        z = position->z >> 8;
        if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
            p->value = 0;
        else
            p->value = gUnk_030052F4->rows[z] + x;
    }
    p->active = 1;
    return 0;
}
