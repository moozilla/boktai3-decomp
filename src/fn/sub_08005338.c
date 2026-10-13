#include "global.h"
#define Scene_UpdateObjectDisappearance sub_08005338
struct Draw {
    u8 pad[16];
    s16 mode;
};
struct Object {
    u8 type, flags;
    u16 timer, kind;
    u8 pad6[11];
    u8 entered;
    u8 pad12[7];
    u8 mode;
    u8 pad1A[6];
    u8 phase, pad21[7];
    u32 script, id;
    u8 pad30[8];
    s16 x, y, z;
    u8 pad3E[82];
    struct Draw draw;
    u8 padA4[30];
    u16 terrainFlags;
};
struct Args {
    u32 count : 16;
    s32 *values;
};
void sub_0822B2F8(s32);
void sub_0821AD08(u32, struct Args *);
s32 Mod(s32, s32);
static inline void setMode(struct Draw *draw, s16 mode)
{
    draw->mode = mode;
}
void Scene_UpdateObjectDisappearance(struct Object *p)
{
    s32 values[5];
    struct Args args;
    if (p->entered) {
        p->entered = 0;
        p->phase = 0;
        setMode(&p->draw, 0);
        sub_0822B2F8(0x136);
        p->terrainFlags |= 0x21;
        if (p->script) {
            values[0] = p->id;
            values[1] = p->type;
            values[2] = p->x;
            values[3] = p->y;
            values[4] = p->z;
            args.count = 5;
            args.values = values;
            sub_0821AD08(p->script, &args);
            p->script = 0;
        }
    }
    if (!p->phase) {
        if (!Mod(p->timer, 2)) {
            if (!p->draw.mode)
                setMode(&p->draw, 1);
            else
                setMode(&p->draw, 0);
        }
        if (p->timer++ > 29) {
            p->timer = 0;
            p->phase++;
        }
    } else {
        if (p->timer++ > 5) {
            p->timer = 0;
            if (++p->phase > 3)
                p->mode = 2;
            else
                setMode(&p->draw, p->phase);
        }
    }
}
