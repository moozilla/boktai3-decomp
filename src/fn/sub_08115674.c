// MATCHING_CONCESSION: register-pinning
#include "display_animation.h"
#include "background_controller.h"
// Per-frame update of a pooled collectible: animates, tests proximity and handles pickup hit.
struct Render {
    u32 flags;
    u8 pad4[12];
    u16 frame;
};
struct Col {
    u8 pad0[0x1C];
    struct Render render;
    u8 pad30[0x38 - 0x30];
    s16 screenX, screenY, screenZ;
    u8 pad3E[0x48 - 0x3E];
    struct DisplaySequenceState sequence;
    u8 pad58[0x9C - 0x58];
    u32 entryFlags;
    u8 pad9C[0xA8 - 0xA0];
    u16 hit;
    u8 hitFlag;
    u8 padAB[0xDC - 0xAB];
    s16 x, y, z;
    u8 padE2[0xEC - 0xE2];
    s16 hp;
    u8 padEE[1];
    u8 flagEF, padF0, tick;
};
void SpatialIndex_SetPosition(void *, s32, s32, s32);
void SpatialIndex_InsertEntry(void *);
u8 Collectible_IsNearPlayer(void *);
void Collectible_StartDeath(void);
void Sound_PlaySong(u32);
void AnimData_SelectPaletteBank(void *, s32);
void Collectible_UpdateEntry(struct Col *p)
{
    struct DisplaySequenceState *state;
    const volatile u16 *command;
    u32 duration, flags, bit;
    struct Render *render;
    u32 *flagsPtr;
    u8 *ep;
    if (p->tick) {
        s16 *yp, *zp, *xp;
        void *entry;
        yp = &p->y;
        p->screenY = *yp - *(s32 *)(((u8 *)gSceneCamera) + 0x48);
        zp = &p->z;
        p->screenZ = *zp;
        p->render.flags &= ~1;
        {
            void *e = (u8 *)p + 0x5C;
            s32 x, y, z;
            xp = &p->x;
            x = *xp;
            y = *yp;
            y -= 0x60;
            z = *zp;
            SpatialIndex_SetPosition(e, x, y - z, z);
        }
        entry = (u8 *)p + 0x9C;
        {
            s32 x = *xp;
            s32 y = *yp;
            s32 z;
            y -= 0x60;
            z = *zp;
            SpatialIndex_SetPosition(entry, x, y - z, z);
        }
        SpatialIndex_InsertEntry(entry);
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
            register u32 mask asm("r5");
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

        p->tick = 0;
    }
    p->screenX = p->x - *(s32 *)(((u8 *)gSceneCamera) + 0x44);
    if (Collectible_IsNearPlayer(p)) {
        u32 *f;
        u32 m;
        SpatialIndex_InsertEntry((u8 *)p + 0x5C);
        f = &p->entryFlags;
        m = ~1;
        *f &= m;
        flagsPtr = f;
    } else {
        u32 *f = &p->entryFlags;
        u32 one = 1;
        *f |= one;
        flagsPtr = f;
    }
    {
        register u32 d asm("r1");
        u8 *hp;
        register u32 hit asm("r5");
        asm("" : "=r"(d));
        hp = &p->hitFlag;
        hit = *hp;
        ep = &p->flagEF;
        asm("" ::"r"(d));
        if (hit == 1) {
            p->hp -= p->hit;
            *hp = 0;
            p->hit = 0;
            *ep = 8;
            if (p->hp <= 0) {
                *flagsPtr |= hit;
                *(u32 *)((u8 *)p + 0xF4) = (u32)Collectible_StartDeath;
                p->tick = hit;
            } else {
                Sound_PlaySong(0x13E);
            }
        }
    }
    if (*ep) {
        AnimData_SelectPaletteBank(p, 0x192);
        (*ep)--;
    } else {
        AnimData_SelectPaletteBank(p, 0x104);
    }
}
