// MATCHING_CONCESSION: register-pinning
#include "display_animation.h"
#include "background_controller.h"
// Collectible defeat: launches a bouncing death animation, then expires.
struct Render {
    u32 flags;
    u8 pad4[12];
    u16 frame;
};
struct Col {
    u8 pad0[0x1C];
    struct Render render;
    u8 pad30[0x38 - 0x30];
    s16 screenX, screenY;
    u8 pad3C[0x48 - 0x3C];
    struct DisplaySequenceState sequence;
    void *animData;
    u8 pad5C[0x9C - 0x5C];
    u32 entryFlags;
    u8 padA0[0xDC - 0xA0];
    s16 x, y, z;
    u8 padE2[2];
    s16 vx, vy;
    u8 padE8[0xEE - 0xE8];
    u8 kind;
    u8 padEF[2];
    u8 active;
    u16 timer;
    void (*callback)(u8 *);
};
void SpatialIndex_RemoveEntry(void *);
void ActorParticles_SpawnAlternatingBurst(void *);
s32 Terrain_GetCellClearance(void *);
void Sprite_BlinkUntilExpired(void *, u32, u32, u32);
s32 Div(s32, s32);
void Collectible_OnDeactivate(u8 *);
void Collectible_StartDeath(struct Col *p)
{
    struct DisplaySequenceState *state;
    const volatile u16 *command;
    u32 duration, flags, bit;
    struct Render *render;
    if (p->active) {
        u32 a, b;
        p->vx = 0x60;
        p->vy = -80;
        p->active = 0;
        switch (p->kind) {
            case 0:
                a = 5;
                b = 0;
                Sound_PlaySong(0x8D);
                break;
            case 2:
                a = 5;
                b = 1;
                Sound_PlaySong(0x8D);
                break;
            case 1:
                a = 3;
                b = 1;
                Sound_PlaySong(0x3E7);
                break;
            default:
                a = 0;
                b = 0;
                break;
        }
        state = &p->sequence;
        {
            void *rd = p->animData;
            u32 ra = a;
            u32 rb = b;
            asm("" : "+r"(rd), "+r"(ra), "+r"(rb));
            Anim_SetSequence(state, rd, ra, rb, 0);
        }
        render = &p->render;
        state = &p->sequence;
        {
            u32 offset = state->index << 1;
            const volatile u16 *table;
            table = p->sequence.commands;
            command = (const volatile u16 *)((u32)table + offset);
        }
        render->frame = *command >> 6;
        {
            u32 input = state->flags;
            register u32 first asm("r1");
            register u32 maskBit asm("r3") = 1;
            bit = maskBit;
            first = bit;
            first &= input;
            if (first != (((*command & 0x30) >> 4) & bit))
                p->render.flags |= 4;
            else
                p->render.flags &= ~4;
        }
        {
            u32 input = state->flags;
            u32 bit;
            register u32 first asm("r1");
            bit = 2;
            asm("" : "+r"(bit));
            first = bit;
            first &= input;
            asm("" : "+r"(first));
            first = (u8)first;
            if (first != (((*command & 0x30) >> 4) & bit))
                render->flags |= 8;
            else
                render->flags &= ~8;
        }
        {
            u32 elapsed = state->elapsed + 1;
            register u32 mask asm("r6");
            register u32 temp asm("r1");
            register u32 savedDuration asm("r3");
            state->elapsed = elapsed;
            temp = 0xFFFF;
            asm("" : "+r"(temp));
            mask = temp;
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
                        if (hold) {
                            state->elapsed = savedDuration;
                        } else {
                            state->index = state->frameCount - 1;
                        }
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
                            {
                                u32 lastIndex = frameCount - 1;
                                state->index = lastIndex;
                            }
                        } else {
                            state->index = 0;
                        }
                    } else {
                    }
                }
                {
                    u32 offset = state->index << 1;
                    const volatile u16 *table;
                    table = state->commands;
                    command = (const volatile u16 *)((u32)table + offset);
                }
                state->frameWeight = *command & 15;
                {
                    register s32 weight asm("r1") = state->frameWeight;
                    s32 speed;
                    speed = state->speed;
                    speed = (speed * weight) >> 6;
                    state->duration = speed;
                    if (!(u8)speed)
                        state->duration = 1;
                }
            }
        }

        p->timer = 0;
        SpatialIndex_RemoveEntry((u8 *)p + 0x9C);
        ActorParticles_SpawnAlternatingBurst(p);
        AnimData_SelectPaletteBank(p, 0x104);
    }
    {
        s16 *xp, *vxp, *yp, *vyp;
        u16 *tp;
        s32 c;
        register u32 d asm("r0");
        register u32 d1 asm("r1");
        asm("" : "=r"(d));
        xp = &p->x;
        vxp = &p->vx;
        asm("" ::"r"(d), "r"(d1));
        *xp += *vxp >> 4;
        yp = &p->y;
        vyp = &p->vy;
        *yp += *vyp >> 4;
        *vyp += 4;
        c = Terrain_GetCellClearance(xp);
        if (c < *yp) {
            *yp = c;
            *vyp = Div(-*vyp * 4, 10);
            *vxp = Div(*vyp * 4, 10);
        }
        p->screenX = *xp - *(s32 *)(((u8 *)gSceneCamera) + 0x44);
        p->screenY = *yp - *(s32 *)(((u8 *)gSceneCamera) + 0x48);
        tp = &p->timer;
        if (*tp > 0x1E)
            Sprite_BlinkUntilExpired((u8 *)p + 0x1C, *tp, 0x32, 0x50);
        if (*tp > 0x64) {
            p->callback = Collectible_OnDeactivate;
            p->active = 1;
        } else {
            (*tp)++;
        }
    }
}
