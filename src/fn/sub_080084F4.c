#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {
    s16 x, y, z, pad;
};
struct Graphics {
    u32 pad;
    u16 base;
};
struct Sprite {
    u32 flags;
    u8 pad4[4];
    u16 tile;
    u8 width, height;
    u8 padC[3];
    u8 mode, priority;
    u8 pad11[7];
    struct Vec position;
    u8 pad20[8];
};
struct Object {
    u32 flags;
    u8 pad4[24];
    struct Vec position;
    u8 pad24[8];
};
struct Trigger {
    s16 id, kind;
    s8 actor;
    u8 type, flags, flip, persistent, started, visible, flash, tick, cooldown, armed, shake;
    s16 baseX, baseZ;
    u8 option, condition;
    u16 sound;
    u32 timer;
    struct Sprite *sprite;
    struct Object *object;
    void *other, *animation;
    struct Vec *position;
    u32 startScript, script, arguments[3];
    void (*update)(void *, struct Trigger *);
    u8 effect[108];
    struct Trigger *previous, *next;
};
struct Manager {
    u8 task[24];
    struct Graphics *graphics;
    void *resource;
    u16 kind;
};
struct Manager *sub_08008270(void);
void *sub_08219C40(u32);
void sub_08219DD8(void *, u32);
void sub_08219D38(void *);
void sub_08007444(void *, struct Trigger *);
void sub_08217DE0(struct Sprite *, struct Graphics *, u32);
void sub_08217EC4(struct Sprite *, s32, s32);
s32 sub_0821D6F4(struct Vec *, s32);
s32 sub_0822C034(struct Vec *);
void sub_08013930(void *, struct Vec *, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void sub_08006FAC(struct Trigger *);
struct Map {
    u32 pad0;
    u8 *cells;
    u8 pad8[28];
    u16 rows[1];
};
extern struct Map *gUnk_030052F4;
extern u32 gUnk_030052F8, gUnk_030052FC;
u8 *sub_0821D64C(s32, s32);
static inline s32 cellIndex(struct Vec *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline s32 groundHeight(struct Vec *position)
{
    s32 index = cellIndex(position), height;
    u8 *cell = sub_0821D64C(index, 1);
    u32 kind;
    if (cell)
        cell += 4;
    else
        cell = gUnk_030052F4->cells + 12 + index * 4;
    kind = *cell >> 4;
    height = (*cell & 15) << 8;
    switch (kind) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}
struct State {
    u8 pad0[0x448];
    u32 flags;
    u8 pad44C[0xE8];
    u16 mode, pad536, active;
};
extern struct State *gUnk_02000580;
extern u32 gUnk_030053F4;
extern s32 gUnk_02000478;
extern u16 gUnk_02000534, gUnk_02000500;
static inline s32 stateMode(void)
{
    struct State *state = gUnk_02000580;
    if (state) {
        u32 active = state->active;
        s32 yes = 0;
        if (active && state->mode == 4)
            yes = 1;
        return yes;
    }
    return 0;
}
static inline s32 globalMode(void)
{
    u32 active = gUnk_02000534;
    s32 yes = 0;
    if (active && gUnk_02000500 == 4)
        yes = 1;
    return yes;
}
static inline s32 actorMode(void)
{
    struct State *state = gUnk_02000580;
    if (state) {
        u32 mask = 16;
        return state->flags & mask;
    }
    return 0;
}
void sub_080075F8(void *, struct Trigger *);
void sub_080077FC(void *, struct Trigger *);
void sub_0822B2F8(s32);
s32 sub_082151E4(void *, u16);
void sub_082144A4(struct Object *, void *, s32);
void sub_08215284(void *, s32);
void sub_08220C8C(void *, void *, s32, s32, s32);
void sub_08013728(void *);
#define Trigger_CreateContainer sub_080084F4
s32 Trigger_CreateContainer(struct Vec *position, s32 id, s32 kind, s32 flip, s32 bob,
                            u32 startScript, u32 script, u32 *arguments, s32 sound, s32 hidden,
                            s32 condition)
{
    struct Manager *manager = sub_08008270();
    struct Trigger *p;
    s32 i;
    u32 *in, *out;
    if (!manager)
        return -1;
    p = sub_08219C40(188);
    if (!p)
        return -1;
    sub_08219DD8(p, 188);
    p->id = id;
    p->kind = kind;
    p->actor = -1;
    p->type = 1;
    p->flags = 0;
    p->flip = flip;
    p->timer = 0;
    p->sprite = 0;
    p->object = 0;
    p->other = 0;
    p->animation = 0;
    if (bob) {
        p->update = sub_080075F8;
        if (!(gUnk_030053F4 & 2048))
            sub_0822B2F8(623);
    } else
        p->update = sub_080077FC;
    p->startScript = startScript;
    p->script = script;
    in = arguments;
    out = p->arguments;
    i = 2;
    do {
        *out++ = *in++;
    } while (--i >= 0);
    p->object = sub_08219C40(44);
    if (!p->object) {
        sub_08219D38(p);
        return -1;
    }
    sub_08219DD8(p->object, 44);
    p->other = sub_08219C40(28);
    if (!p->other) {
        if (p->object)
            sub_08219D38(p->object);
        sub_08219D38(p);
        return -1;
    }
    sub_08219DD8(p->other, 28);
    p->animation = sub_08219C40(16);
    if (!p->animation) {
        if (p->object)
            sub_08219D38(p->object);
        if (p->other)
            sub_08219D38(p->other);
        sub_08219D38(p);
        return -1;
    }
    sub_08219DD8(p->animation, 16);
    if (!sub_082151E4(p->other, manager->kind)) {
        if (p->object)
            sub_08219D38(p->object);
        if (p->other)
            sub_08219D38(p->other);
        if (p->animation)
            sub_08219D38(p->animation);
        sub_08219D38(p);
        return -1;
    }
    sub_082144A4(p->object, p->other, 0);
    p->object->position = *position;
    p->object->position.y = groundHeight((struct Vec *)&p->object->position);
    if (p->flip)
        p->object->flags |= 4;
    p->position = &p->object->position;
    p->started = hidden;
    p->condition = condition;
    if ((u8)condition == 0)
        sub_08215284(p->other, 772);
    else if ((u8)condition == 1)
        sub_08215284(p->other, 775);
    else
        sub_08215284(p->other, 776);
    {
        s32 hidden = p->started;
        if (hidden) {
            s32 available;
            p->object->flags |= 1;
            if (gUnk_030053F4 & 2048) {
                available = stateMode();
            } else {
                if (gUnk_02000478 > 0)
                    goto reveal;
                if (gUnk_02000580 && (({
                        u32 *flags = &gUnk_02000580->flags;
                        u32 mask = 16;
                        *flags &mask;
                    })))
                    goto reveal;
                available = globalMode();
            }
            asm("" : : "r"(available));
            if (!available)
                goto conceal;
        reveal:
            hidden = 0;
            goto visibilityDone;
        conceal:
            hidden = 1;
        }
    visibilityDone:
        p->visible = hidden;
    }
    {
        void *animation = p->animation;
        void *resource = ({
            struct Manager *m = manager;
            m->resource;
        });
        register s32 zero asm("r2") = 0;
        sub_08220C8C(animation, resource, zero, 0, zero);
    }
    {
        s32 value;
        if (({
                register s32 input asm("r3") = sound;
                input;
            }))
            value = sub_0822C034(&p->object->position);
        else
            value = *(u16 *)&sound;
        p->sound = value;
    }
    sub_08013930(p->effect, &p->object->position, 64, 24, 40, 32, 512, 1, 5, 1, ({
                     register s32 zero asm("r0") = 0;
                     zero;
                 }));
    sub_08013728(p->effect);
    sub_08006FAC(p);
    return 0;
}
