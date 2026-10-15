#include "global.h"
// MATCHING_CONCESSION: register-pinning
/* Tentative role: initialize and advance an animation with a previous-state callback.
 * Cleanup: local flag, flip-mask, frame-factor and timer register hints. */
struct Obj {
    u32 flags;
    u8 pad[4];
    s8 scaleX, scaleY;
    u8 padScale[6];
    u16 frame;
    u8 pad2[12];
    u16 angle;
};
struct Anim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unused, scale, timer;
};
struct S {
    u8 p0[0xddc];
    struct Obj obj;
    u8 p1a[0xe08 - 0xdfc];
    u8 effect[0xe24 - 0xe08];
    struct Anim anim;
    u8 p2[0xe74 - 0xe34];
    u32 arg;
    u16 count;
    s8 delta;
    u8 p3a[0xf50 - 0xe7b];
    void *handle;
    u8 p3b[4];
    u8 done, state;
    u16 mode : 4;
    u16 phase : 12;
    u16 oldMode;
    u8 p4[0xf64 - 0xf5e];
    u8 flags;
    u8 p5[7];
    u8 last, next;
    u8 p6a[0xf86 - 0xf6e];
    u8 savedFrame, savedTimer;
    u8 p6b[4];
    void (*previous)(void *);
    void (*fn)(void *);
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
    u32 value = v;
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
void sub_081E682C(struct S *p)
{
    struct Obj *o;
    struct Anim *a;
    const u16 *entry;
    u32 flag;
    u16 n;
    u8 done;
    if (take(p)) {
        ScriptUtility_ClearControlFlag();
        ActorShadow_Disable(p->p2);
        p->obj.flags |= 1;
        p->oldMode = p->mode;
        {
            u32 mode = 1;
            p->mode = mode;
        }
        {
            register u32 raw asm("r1") = *(u16 *)((u8 *)p + 0xf5a);
            register u32 mask asm("r2") = 15;
            *(u16 *)((u8 *)p + 0xf5a) = (mask & raw) | 32;
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
        {
            u32 active = 4;
            register void (*fn)(void *, void *) asm("r4") = sub_081E5608;
            u32 reset = 1;
            struct Child *child = (struct Child *)((u8 *)p + 0xc8);
            s32 i = 15;
            do {
                u32 v = child->flags;
                if (active & v) {
                    register u32 flags asm("r0");
                    child->fn = fn;
                    flags = reset | v;
                    child->flags = flags;
                }
                child++;
            } while (--i >= 0);
        }
    }
}
