// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
#include "scene_vec.h"
struct Frames {
    u16 *data;
    u8 flags, count, multiplier, duration;
    u16 index, padA, rate, timer;
};
struct ScriptSprite {
    u32 flags;
    u8 pad4[3];
    u8 priority;
    u8 pad8[8];
    u16 frame;
    u8 pad12[0xA];
    struct SceneVec position;
};
struct ScriptResource {
    u8 unknown0[6];
    u16 paletteIndex;
    u8 unknown8[4];
    const void *paletteData;
    u8 unknown10[0xC];
    struct Frames frames;
    u16 palette[16];
};
struct SceneScriptObject {
    u8 task[0x18];
    struct ScriptSprite sprite;
    u8 pad3C[8];
    struct ScriptResource resource;
    u8 pad90[0];
    void *proc;
    u8 pad94[0xE8 - 0x94];
    u32 caller;
    s16 state;
    s16 lifetime;
    s16 reveal;
    u16 fadeTimer;
    u16 holdTimer;
    s16 parameter;
    u32 script;
    u8 shadow[0x13C - 0xFC];
    u16 finished;
    u16 counter;
};
s32 Script_SeekToKeyword(int);
s32 Script_GetValue(void);
void SceneScriptObject_InitHitCollider(struct SceneScriptObject *);
void Shadow_Initialize(void *, void *, u32);
void CpuSet(const void *, void *, u32);
static inline s32 PaletteIndex(s32 state, s32 parameter)
{
    if (state <= 1) {
        if (parameter >= 0)
            return parameter + 0x26B;
        return 0x26B;
    }
    return ((state - 2) >> 1) + 0x26B;
}
s32 sub_081B866C(struct SceneScriptObject *p, u32 caller)
{
    struct ScriptSprite *sprite = &p->sprite;
    struct Frames *f;
    const vu16 *frame;
    p->caller = caller;
    if (Script_SeekToKeyword('r'))
        p->reveal = Script_GetValue();
    if (Script_SeekToKeyword('p')) {
        sprite->position.x = Script_GetValue();
        sprite->position.y = Script_GetValue();
        sprite->position.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('l'))
        p->lifetime = Script_GetValue();
    else
        p->lifetime = 2;
    if (Script_SeekToKeyword('b'))
        p->script = Script_GetValue();
    else
        p->script = 0;
    if (Script_SeekToKeyword('t'))
        p->state = Script_GetValue();
    else
        p->state = 0;
    if (Script_SeekToKeyword('P'))
        p->parameter = Script_GetValue();
    else
        p->parameter = -1;
    {
        s32 ok;
        if (p->state == 14)
            ok = AnimData_Load(&p->resource, 0x6BEA);
        else
            ok = AnimData_Load(&p->resource, 0xB07C);
        if (!ok)
            return -1;
    }
    Sprite_Init(sprite, &p->resource, 0);
    p->proc = ScriptProc_Start(0x922E, 0xB5F1);
    sprite->priority = 2;
    if (p->state == 14) {
        sprite->frame = 0;
        if (p->reveal)
            sprite->flags |= 4;
    } else {
        f = &p->resource.frames;
        Anim_SetSequence(f, p->proc, (u16)p->state, 0, (u8)p->reveal);
        frame = &f->data[f->index];
        sprite->frame = *frame >> 6;
        {
            u32 input = f->flags;
            u32 one = 1;
            u32 first;
            asm("" : "=r"(first) : "0"(one));
            first &= input;
            if (first != (((*frame & 0x30) >> 4) & one))
                sprite->flags |= 4;
            else
                sprite->flags &= ~4;
        }
        {
            u32 flags = f->flags;
            u32 two = 2;
            u32 first;
            asm("" : "=r"(first) : "0"(two));
            first &= flags;
            asm("" : "+r"(first));
            if ((u8)first != (((*frame & 0x30) >> 4) & two))
                sprite->flags |= 8;
            else
                sprite->flags &= ~8;
        }
        if (++f->timer >= f->duration) {
            f->timer = 0;
            if (f->flags & 4) {
                u32 index = f->index;
                if (index == 0) {
                    if (f->flags & 8) {
                        f->timer = f->duration;
                        goto next;
                    }
                    index = f->count;
                }
                f->index = index - 1;
            } else if (++f->index >= f->count) {
                if ((u8)(f->flags & 8)) {
                    f->timer = f->duration;
                    f->index = f->count - 1;
                } else
                    f->index = 0;
            }
        next:
            frame = &f->data[f->index];
            f->multiplier = *frame & 0xF;
            f->duration = (f->multiplier * f->rate) >> 6;
            if (!f->duration)
                f->duration = 1;
        }
    }
    p->resource.paletteIndex = PaletteIndex(p->state, p->parameter);
    p->resource.paletteData = p->resource.palette;
    {
        u32 mode = 0x04000008;
        s32 bank = PaletteIndex(p->state, p->parameter);
        p->resource.paletteIndex = bank;
        CpuSet(gPaletteBankData + bank * 32, p->resource.palette, mode);
    }
    SceneScriptObject_InitHitCollider(p);
    if (p->state == 12) {
        sprite->position.y += 0xC0;
        Shadow_Initialize(p->shadow, &sprite->position, 1);
        ActorShadow_Enable(p->shadow);
    }
    p->fadeTimer = 0;
    p->holdTimer = 0;
    p->finished = 0;
    p->counter = 0;
}
