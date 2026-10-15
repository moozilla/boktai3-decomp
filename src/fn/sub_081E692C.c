#include "global.h"
// Provisional name: Actor_MoveAlongCurveToRest
// MATCHING_CONCESSION: register-pinning
/* Tentative role: initialize and advance an animation with a previous-state callback.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */
struct Obj {
    u32 flags;
    u8 pad[4];
    s8 scaleX, scaleY;
    u8 padScale[6];
    u16 frame;
    u8 pad2[10];
    s16 x, y, z;
};
struct Anim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unused, scale, timer;
};
struct S {
    u8 p0[0xddc];
    struct Obj obj;
    u8 p1a[0xe08 - 0xe00];
    u8 effect[0xe24 - 0xe08];
    struct Anim anim;
    u8 p2[0xe74 - 0xe34];
    u32 arg;
    u16 count;
    s8 delta;
    u8 p3a[0xed0 - 0xe7b];
    s16 targetX, targetY, targetZ;
    u8 gapD6[2];
    s16 startX, startY, startZ;
    u8 gapDE[0xf50 - 0xede];
    void *handle;
    u8 p3b[4];
    u8 done, state;
    u16 mode : 4;
    u16 phase : 12;
    u16 oldMode;
    u16 speed, heading;
    u8 gap62[2];
    u8 flags;
    u8 gap65;
    u8 direction;
    u8 gap67[2];
    s8 bend;
    u8 gap6a;
    u8 flip;
    u8 last, next;
    u8 p6a[0xf86 - 0xf6e];
    u8 savedFrame, savedTimer;
    u8 p6b[4];
    void (*previous)(void *);
    void (*fn)(void *);
    void (*scaleFn)(void *);
    void (*otherFn)(void *);
    s32 timer;
    u32 variant;
};
void Anim_SetSequence(void *, u32, u32, u32, u32);
void sub_081E4188(void *);
void ActorShadow_Disable(void *);
void sub_081E41EC(void *);
static inline u8 take(struct S *p)
{
    s32 m = 1;
    register s32 v asm("r1") = p->flags;
    if (v & m) {
        p->flags = ~m & v;
        p->done = 0;
        return 1;
    }
    return 0;
}
static inline u32 mode_bits(u8 *p, u32 mask)
{
    register u32 v asm("r1") = *p;
    u32 result = mask & v;
    return result;
}
static inline u32 frame_factor(u16 v)
{
    register u32 value asm("r1") = v;
    u32 mask = 15;
    return mask & value;
}
struct Child {
    void (*fn)(void *, void *);
    u8 pad;
    u8 flags;
    u8 rest[0xdc - 6];
};
void sub_081E5608(void *, void *);
void ScriptUtility_ClearControlFlag(void);
void sub_081E4174(void *);
void AnimData_SelectPaletteBank(void *, s32);
void Sound_PlaySong(s32);
void Script_Call(void *, s32);
void ActorShadow_Enable(void *);
void ActorPalette_BeginFade(void *);
extern u8 gUnk_03005430[];
extern const s16 gUnk_086149C4[];
u8 Math_Atan2(s32, s32);
void sub_081E41B0(void *);
static inline void set_direction(s16 angle, u8 *directionOut, u8 *flipOut)
{
    register s32 direction asm("r2");
    while (angle < 0)
        angle += 256;
    while (angle > 255)
        angle -= 256;
    {
        register s32 shifted asm("r0") = angle - 16;
        direction = (shifted >> 5) & 7;
    }
    {
        s32 choice = direction;
        if (choice == 4)
            *directionOut = 0;
        else if (choice == 0)
            *directionOut = 4;
        else if (choice < 4) {
            *directionOut = 4 - direction;
            *flipOut = 1;
        } else {
            register s32 result asm("r0") = direction - 4;
            *directionOut = result;
            *flipOut = 0;
        }
    }
}
void Actor_MoveAlongCurveToRest(struct S *p)
{
    struct Obj *o;
    struct Anim *a;
    const u16 *entry;
    register u16 n asm("r0");
    u8 done;
    if (take(p)) {
        p->timer = 0;
        p->startX = p->obj.x;
        p->startY = p->obj.y;
        p->startZ = p->obj.z;
        p->state = 0;
        p->bend = (p->variant & 1) ? 1 : -1;
        ActorShadow_Disable(p->p2);
        if (*((u8 *)p + 0xf6f) && p->obj.scaleX != 127 && (p->obj.flags & 16))
            p->scaleFn = sub_081E41B0;
        if (!*((u8 *)p + 0xf77)) {
            p->targetX = 25;
            p->targetY = 102;
            p->targetZ = 0;
        } else if (gUnk_03005430[14] != 5 && gUnk_03005430[14] != 0 && gUnk_03005430[14] != 4) {
            p->targetX = 25;
            p->targetY = 92;
            p->targetZ = 0;
        } else {
            p->targetX = 25;
            p->targetY = 75;
            p->targetZ = 0;
        }
    }
    p->heading = Math_Atan2(p->targetX - p->obj.x, p->targetY - p->obj.y);
    o = &p->obj;
    a = &p->anim;
    entry = a->data + a->frame;
    o->frame = *entry >> 6;
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
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
                    done = 1;
                } else {
                    a->frame = 0;
                    done = 1;
                }
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
    p->done = done;
    if (p->timer++ > 31) {
        p->obj.x = p->targetX;
        p->obj.y = p->targetY;
        p->oldMode = p->mode;
        {
            u32 mode = 3;
            p->mode = mode;
        }
        {
            register u32 raw asm("r1") = *(u16 *)((u8 *)p + 0xf5a);
            register u32 mask asm("r2") = 15;
            *(u16 *)((u8 *)p + 0xf5a) = (mask & raw) | 16;
            if (p->oldMode == 3) {
                register u32 bits asm("r0") = mode_bits((u8 *)p + 0xf5a, mask);
                if (bits == 3) {
                    p->savedFrame = p->anim.frame;
                    p->savedTimer = p->anim.timer;
                }
            }
        }
        {
            register s32 mask asm("r0") = 1;
            register s32 v asm("r1") = p->flags;
            p->flags = mask | v;
        }
        if (p->handle) {
            void *handle = p->handle;
            p->handle = 0;
            Script_Call(handle, 0);
        }
    } else {
        set_direction(p->heading - 16, &p->direction, &p->flip);
        Anim_SetSequence(&p->anim, p->arg, p->state, p->direction, p->flip);
        {
            s32 delta = p->targetX - p->startX;
            p->obj.x = p->startX + Div(delta * p->timer, 32);
        }
        {
            s32 delta = p->targetY - p->startY;
            s32 y = (u16)p->startY + Div(delta * p->timer, 32);
            s32 bend = p->bend;
            p->obj.y = y + bend * (gUnk_086149C4[(p->timer * 4) & 255] >> 7);
        }
    }
}
