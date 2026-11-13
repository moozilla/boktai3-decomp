#include "display_sequence_sprite.h"
#include "background_controller.h"
struct Camera {
    u8 unknown0[0x44];
    s32 screenX, screenY;
};
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
            Anim_SetSequence(state, rd, (u8)ra, (u8)rb, 0);
        }
        render = &p->render;
        DisplaySequence_StepSprite((struct DisplaySequenceSprite *)render, &p->sequence);

        p->timer = 0;
        SpatialIndex_RemoveEntry(&p->entryFlags);
        ActorParticles_SpawnAlternatingBurst(p);
        AnimData_SelectPaletteBank(p, 0x104);
    }
    {
        s16 *xp, *vxp, *yp, *vyp;
        u16 *tp;
        s32 c;
        xp = &p->x;
        vxp = &p->vx;
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
        p->screenX = *xp - gSceneCamera->screenX;
        p->screenY = *yp - gSceneCamera->screenY;
        tp = &p->timer;
        if (*tp > 0x1E)
            Sprite_BlinkUntilExpired(&p->render, *tp, 0x32, 0x50);
        if (*tp > 0x64) {
            p->callback = Collectible_OnDeactivate;
            p->active = 1;
        } else {
            (*tp)++;
        }
    }
}
