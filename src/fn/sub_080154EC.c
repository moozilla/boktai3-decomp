#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {
    s16 x, y, z, pad;
};
struct Obj {
    u32 flags;
    u8 pad[12];
    u16 frame;
    u8 pad12[10];
    struct Vec position;
    u8 tail[8];
};
struct Anim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unused, scale, timer;
};
struct Piece {
    u8 active, pad1;
    u16 timer;
    s16 verticalScale, horizontalScale;
    u16 verticalStart, verticalDecay, horizontalStart, horizontalDecay;
    struct Vec direction, velocity;
    u8 resource[28];
    struct Obj sprite;
    struct Anim anim;
};
struct Manager {
    u8 pad[24];
    void *resources[2];
    u32 timer;
    struct Piece pieces[32];
};
void Sprite_Remove(void *);
extern struct Manager *gUnk_02000068;
extern u16 gUnk_08604C54[];
void *ScriptProc_Start(int, int);
int AnimData_Load(void *, int);
void Sprite_InitUnlinked(void *, void *, int);
void Anim_SetSequence(void *, void *, int, int, int);
extern u32 gRandomIndex;
extern u16 gUnk_0203B400[];
extern s16 gUnk_086149C4[];
struct Config {
    u16 type, flags, pad, speed;
    s16 vertical, verticalRange, horizontal, horizontalRange;
    u16 pitch, pitchRange, yaw, yawRange, verticalStart, verticalStartRange, verticalDecay,
        verticalDecayRange, horizontalStart, horizontalStartRange, horizontalDecay,
        horizontalDecayRange;
};
struct Manager *AnimatedParticleManager_Create(int, int);
int Mod(int, int);
void AnimData_SelectPaletteBank(void *, int);
int AnimatedParticle_PriorityFromFlags(int);
void Anim_SetDurationScale(void *, int);
void Sprite_LinkToList(void *, int);
static inline int rnd(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return gUnk_0203B400[gRandomIndex];
}
static inline int scale12(int a, int b)
{
    int x = a * b;
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}
static inline void setFrame(struct Obj *o, int f)
{
    o->frame = f;
}
static inline u32 frame_factor(u16 v)
{
    register u32 value asm("r1") = v;
    u32 mask = 15;
    return mask & value;
}
int AnimatedParticles_Create(int count, struct Vec *pos, struct Config *c)
{
    int i, emitted;
    u16 *table;
    u32 *index;
    if (gUnk_02000068)
        goto start;
    if (AnimatedParticleManager_Create(0, 0))
        goto start;
    goto failure;
success:
    return 0;
start:
    emitted = 0;
    i = 0;
    table = gUnk_0203B400;
    index = &gRandomIndex;
    for (; i <= 31; i++) {
        struct Piece *p = &gUnk_02000068->pieces[i];
        struct Obj *o = &p->sprite;
        if (!p->active) {
            int pitch, yaw;
            struct Anim *a;
            register struct Anim *saved asm("r8");
            const u16 *entry;
            register u16 n asm("r0");
            int done;
            p->active = 1;
            p->pad1 = c->type;
            p->timer = 0;
            if (!c->verticalRange)
                p->verticalScale = c->vertical;
            else
                p->verticalScale =
                    c->vertical + Mod(table[ *index = (*index + 1) & 1023], c->verticalRange);
            if (!c->horizontalRange)
                p->horizontalScale = c->horizontal;
            else
                p->horizontalScale =
                    c->horizontal + Mod(table[ *index = (*index + 1) & 1023], c->horizontalRange);
            if (!c->pitchRange)
                pitch = c->pitch;
            else
                pitch = c->pitch + Mod(table[ *index = (*index + 1) & 1023], c->pitchRange);
            if (!c->yawRange)
                yaw = c->yaw;
            else
                yaw = c->yaw + Mod(table[ *index = (*index + 1) & 1023], c->yawRange);
            if (!c->verticalStartRange)
                p->verticalStart = c->verticalStart;
            else
                p->verticalStart = c->verticalStart +
                                   Mod(table[ *index = (*index + 1) & 1023], c->verticalStartRange);
            if (!c->horizontalStartRange)
                p->horizontalStart = c->horizontalStart;
            else
                p->horizontalStart = c->horizontalStart + Mod(table[ *index = (*index + 1) & 1023],
                                                              c->horizontalStartRange);
            if (c->flags & 1)
                o->flags |= 512;
            else {
                o->flags &= ~512;
                o->flags &= ~1024;
            }
            if (c->flags & 2)
                o->flags |= 16;
            else
                o->flags &= ~16;
            p->direction.x = gUnk_086149C4[(yaw + 64) & 255];
            p->direction.y = gUnk_086149C4[pitch & 255];
            p->direction.z = gUnk_086149C4[yaw & 255];
            p->velocity.x = scale12(p->direction.x, p->horizontalScale);
            p->velocity.y = scale12(p->direction.y, p->verticalScale);
            p->velocity.z = scale12(p->direction.z, p->horizontalScale);
            if (p->pad1 == 5) {
                AnimData_Load(p->resource, 0x61f9);
                AnimData_SelectPaletteBank(p->resource, 0x29f);
                setFrame(&p->sprite, 32);
                a = &p->anim;
                Anim_SetSequence(a, gUnk_02000068->resources[1], 6, 0, 0);
            } else if (p->pad1 == 6) {
                AnimData_Load(p->resource, 0x61f9);
                AnimData_SelectPaletteBank(p->resource, 0x2a7);
                setFrame(&p->sprite, 32);
                a = &p->anim;
                Anim_SetSequence(a, gUnk_02000068->resources[1], 6, 0, 0);
                asm("" : : "r"(a), "i"(6));
            } else if (p->pad1 == 7) {
                AnimData_Load(p->resource, 0x61f9);
                AnimData_SelectPaletteBank(p->resource, 0x2a6);
                setFrame(&p->sprite, 45);
                a = &p->anim;
                Anim_SetSequence(a, gUnk_02000068->resources[1], 8, 0, 0);
                asm("" : : "r"(a), "i"(7));
            } else {
                AnimData_Load(p->resource, 0x4384);
                AnimData_SelectPaletteBank(p->resource, p->pad1 + 182);
                a = &p->anim;
                Anim_SetSequence(a, gUnk_02000068->resources[0], 0, 0, 0);
            }
            saved = a;
            asm("" : "+r"(saved));
            a = saved;
            entry = p->anim.data + a->frame;
            o->frame = *entry >> 6;
            {
                u32 input = a->flags;
                u32 left = 1;
                u32 mask = 1;
                left &= input;
                if (left != (((*entry & 0x30) >> 4) & mask))
                    o->flags |= 4;
                else
                    o->flags &= ~4;
            }
            {
                register u32 input asm("r0") = a->flags;
                u32 mask = 2;
                u32 left = (u8)(mask & input);
                if (left != (((*entry & 0x30) >> 4) & mask))
                    o->flags |= 8;
                else
                    o->flags &= ~8;
            }
            n = ++a->timer;
            if (n >= a->rate) {
                a->timer = 0;
                if (a->flags & 4) {
                    if (a->frame == 0) {
                        if (a->flags & 8) {
                            a->timer = a->rate;
                        } else {
                            a->frame = a->count - 1;
                        }
                        done = 1;
                    } else {
                        a->frame--;
                        done = 0;
                    }
                } else {
                    if (++a->frame >= a->count) {
                        if ((u8)(a->flags & 8)) {
                            a->timer = a->rate;
                            a->frame = a->count - 1;
                        } else {
                            a->frame = 0;
                        }
                        done = 1;
                    } else
                        done = 0;
                }
                entry = a->data + a->frame;
                a->factor = frame_factor(*entry);
                a->rate = (a->factor * a->scale) >> 6;
                if (!a->rate)
                    a->rate = 1;
            } else
                done = 0;
            o->position = *pos;
            *((u8 *)o + 7) = AnimatedParticle_PriorityFromFlags(c->flags);
            Anim_SetDurationScale(saved, c->speed);
            Sprite_LinkToList(o, 0);
            if (emitted++ >= count)
                goto success;
        }
    }
failure:
    return -1;
}
