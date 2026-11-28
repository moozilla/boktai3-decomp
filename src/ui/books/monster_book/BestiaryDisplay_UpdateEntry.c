#include "global.h"
#include "ui/books/bestiary_display.h"

extern const s16 gUnk_086149C4[];
void DisplayAnimation_SetFlags(void *, u32);
void DisplayAnimation_ClearFlags(void *, u32);
void PaletteTransition_Restart(void *);
void Dir_Decompose(s32, u8 *, u8 *, u32);
void DisplayAnimation_SetEntry(void *, u32, u32, u32, u32, u32, u32);
void DisplayAnimation_UpdatePlayback(void *);
void DisplayEntry_SetAnim(void *, s32);

static inline s32 Scale(s32 n, s32 bits)
{
    if (n >= 0)
        return n >> bits;
    return -((-n) >> bits);
}

s32 BestiaryDisplay_UpdateEntry(struct BestiaryDisplay *p)
{
    s32 i;
    const struct BestiaryDisplayConfig *config;

    if (p->hidden) {
        for (i = 0; i < 6; i++) {
            if (p->animations[i].active)
                DisplayAnimation_SetFlags(&p->animations[i], 1);
        }
        p->sprite.flags |= 1;
        return 0;
    }
    for (i = 0; i < 6; i++) {
        if (p->animations[i].active)
            DisplayAnimation_ClearFlags(&p->animations[i], 1);
    }
    p->sprite.flags &= ~1;
    p->holdAnimation = 0;
    config = &gUnk_086126E0[p->entry];
    p->config = config;
    if (p->entryChanged) {
        p->swayPhase = 0;
        p->frame = 0;
        p->state = 0;
        PaletteTransition_Restart(p);
    }
    if (config->update)
        config->update(p);
    if (!(config->flags & 4) && !p->holdAnimation) {
        Dir_Decompose(config->directionIndex, &p->dirX, &p->dirY, p->scale);
        DisplayAnimation_SetEntry(p->animations, p->sequence, p->dirX, p->dirY, 1, 0, 5);
        DisplayAnimation_UpdatePlayback(p->animations);
    }
    if (config->flags & 2) {
        s32 angle;
        s32 sway;

        p->swayPhase += config->swaySpeed;
        angle = Scale(p->swayPhase, 5) + 128;
        sway = Scale(config->swayAmplitude * gUnk_086149C4[angle & 255], 13);
        p->animations[0].position[1] = p->origin[1] + sway;
    }
    if (!config->overlay) {
        p->sprite.flags |= 1;
    } else {
        p->sprite.flags &= ~1;
        DisplayEntry_SetAnim(p, config->overlay);
    }
    p->entryChanged = 0;
}
