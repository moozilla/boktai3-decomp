#include "global.h"
#include "render.h"
#include "ram.h"
// MATCHING_CONCESSION: register-pinning
struct DisplaySequenceState {
    const volatile u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unknownA, speed, elapsed;
};
struct ActorColliderView {
    u8 pad[6];
    u16 flags;
};
struct ActorSpriteView {
    u32 flags;
    u8 pad[12];
    u16 frame;
};
struct ScriptCombatActorState {
    u8 p0[0x18];
    s32 timer;
    u8 p1c[0x24 - 0x1c];
    struct ActorSpriteView spr;
    u8 p36[0x40 - 0x38 + 0];
    u8 pos[0x10];
    u8 anim[0x5c - 0x50];
    void *ptr5c;
    u8 p60[0xc0 - 0x60];
    struct ActorColliderView col;
    u8 pc8[0xcc - 0xc8];
    u8 dmgA[8];
    u8 dmgB[0x100 - 0xd4];
    u8 pad100[3];
    u8 b103;
    u16 invuln;
    u8 p106[0x15c - 0x106];
    void *fn15c;
    struct DisplaySequenceState seq;
    u8 p170[0x17c - 0x170];
    u16 hp;
    u8 p17e[2];
    s16 damage;
    u16 w182;
    u8 p184[2];
    s16 dead;
    u8 p188[0x18c - 0x188];
    s16 timer18c;
    u8 p18e[2];
    s16 w190;
    s16 w192;
    s16 w194;
    s16 w196;
    s16 w198;
    u8 p19a[4];
    s16 w19e;
    u8 p1a0[2];
    s16 w1a2;
    u8 p1a4[0x1ac - 0x1a4];
    void *w1ac;
    u8 p1b0[4];
    u32 w1b4;
    u32 w1b8;
    u8 p1bc[0x1c8 - 0x1bc];
    u32 w1c8;
    u32 w1cc;
    u8 w1d0[4];
};
extern u32 gEngineFlags;
extern u32 gUnk_02000560;
void AnimData_SelectPaletteBank(void *, u32 animation);
void Collider_SetPosition(void *collider, void *position, u32 value);
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
void Sound_PlaySong(u32 song);
void TerrainModifiers_UnregisterCell(void *);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
s32 Callback_InvokeActor(void *arg, void *function);
void SceneHud_SetTimedState(void *, int, int, int, int, int);
u32 ScriptCombatActor_UpdateDefeatEffect(void *);
static inline void StepSequence(struct ScriptCombatActorState *owner)
{
    struct ActorSpriteView *sprite = &owner->spr;
    struct DisplaySequenceState *state = &owner->seq;
    const volatile u16 *command = state->commands + state->index;
    register u32 duration asm("r3");
    u32 flags, bit;
    sprite->frame = *command >> 6;
    {
        u32 input = state->flags;
        register u32 first asm("r2");
        u32 one = 1;
        bit = one;
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 4;
        else
            sprite->flags &= ~4;
    }
    {
        u32 input = state->flags;
        u32 bit = 2;
        register u32 first asm("r2");
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != (((*command & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    {
        u32 elapsed = state->elapsed + 1;
        u32 mask;
        register u32 temp asm("r2");
        u32 savedDuration;
        state->elapsed = elapsed;
        temp = 0xFFFF;
        asm("" : "+r"(temp));
        mask = temp;
        asm("" : "+r"(mask));
        duration = state->duration;
        elapsed = (u16)elapsed;
        savedDuration = duration;
        if (elapsed >= duration) {
            state->elapsed = 0;
            flags = state->flags;
            if (flags & 4) {
                u32 index = state->index;
                if (index == 0) {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold)
                        state->elapsed = savedDuration;
                    else
                        state->index = state->frameCount - 1;
                } else {
                    state->index = index - 1;
                }
            } else {
                u32 index = state->index + 1;
                u32 frameCount;
                u32 endFlag;
                state->index = index;
                frameCount = state->frameCount;
                if ((index & mask) >= frameCount) {
                    endFlag = flags & 8;
                    asm("" : "+r"(endFlag));
                    endFlag = (u8)endFlag;
                    if (endFlag) {
                        state->elapsed = duration;
                        state->index = frameCount - 1;
                    } else
                        state->index = 0;
                }
            }
            command = state->commands + state->index;
            state->frameWeight = *command & 15;
            {
                register s32 weight asm("r2") = state->frameWeight;
                s32 speed = state->speed;
                speed = (speed * weight) >> 6;
                state->duration = speed;
                if (!(u8)speed)
                    state->duration = 1;
            }
        }
    }
}

// confidence=low: runs the actor action, damage response, palette timers and sprite playback.
#define ScriptCombatActor_UpdateState sub_08201D38
u32 ScriptCombatActor_UpdateState(struct ScriptCombatActorState *p)
{
    register struct ActorColliderView *c asm("r4");
    u32 m = 0x400;
    if ((gEngineFlags | gScenePauseFlags) & m)
        return 0;
    {
        u32 m2 = 4;
        if (gUnk_02000560 & m2)
            return 0;
    }
    Callback_InvokeActor(p, p->fn15c);
    p->spr.flags &= ~1;
    if (p->dead == 0) {
        c = &p->col;
        {
            register u32 flags asm("r1") = c->flags;
            c->flags = 0xfffb & flags;
        }
        Collider_SetPosition(c, p->pos, 0);
    } else {
        register struct ActorColliderView *disabled asm("r2") = &p->col;
        disabled->flags |= 4;
        p->ptr5c = gPaletteBankData + 0x36e0;
        c = disabled;
    }
    {
        u32 off = 0x180;
        s16 *dp = (s16 *)((u8 *)p + off);
        u16 d = *dp;
        if (*dp > 0 && *(s16 *)((u8 *)p + off + 6) == 0) {
            u16 *hpp = &p->hp;
            *hpp -= d;
            p->w1b4 = p->w1b8;
            Sound_PlaySong(0x13e);
            if (*dp > 0) {
                u32 off2 = 0x19e;
                DamageNumberDisplay_Show(c, *dp, 0, &p->dmgA, &p->dmgB, 1);
                SceneHud_SetTimedState(p->w1ac, (s16)*hpp, p->w198, *(s16 *)((u8 *)p + off2),
                                       *((u8 *)p + off2 - 0x9b) == 4, p->w1a2);
            }
            if ((s16)p->hp <= 0) {
                p->dead = 1;
                if (p->w1c8)
                    TerrainModifiers_UnregisterCell(&p->w1d0);
                Anim_RestartSequence(&p->seq, (void *)p->w1cc, 2, 0, 0);
                AnimData_SelectPaletteBank(p->anim, 0x1b7);
                p->ptr5c = gPaletteBankData + 0x36e0;
                p->timer18c = 0;
                p->timer = 0;
                p->fn15c = ScriptCombatActor_UpdateDefeatEffect;
                Sound_PlaySong(0x529);
            }
            p->timer18c = 8;
            p->w182 = 1;
            p->damage = 0;
        }
    }
    if (p->timer18c > 0 && p->dead == 0) {
        AnimData_SelectPaletteBank(p->anim, 0x192);
        p->timer18c--;
    } else {
        AnimData_SelectPaletteBank(p->anim, 0x1b7);
        if (p->dead == 0) {
            if (p->w190 == 0) {
                AnimData_SelectPaletteBank(p->anim, 0x1b8);
                if (p->w196 > p->w194) {
                    p->w190 = 1;
                    p->w196 = 0;
                }
            } else {
                AnimData_SelectPaletteBank(p->anim, 0x1b7);
                if (p->w196 > p->w192) {
                    p->w190 = 0;
                    p->w196 = 0;
                }
            }
        }
        p->w196++;
        if (p->invuln) {
            p->invuln--;
            if ((p->invuln >> 2) & 1)
                p->spr.flags |= 1;
            else
                p->spr.flags &= ~1;
        }
    }
    StepSequence(p);
    p->timer++;
}
