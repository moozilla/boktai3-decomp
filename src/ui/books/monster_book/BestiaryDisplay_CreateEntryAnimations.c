#include "global.h"
#include "ui/books/bestiary_display.h"

struct EntryIds {
    s32 id[5];
};
extern const struct EntryIds gUnk_0824FC6C;
void Dir_Decompose(s32, u8 *, u8 *, u32);
s32 DisplayAnimation_InitializeKeyframes(void *, u32, u32, void *);
s32 DisplayAnimation_InitializeSequence(void *, u32, u32, u32, void *);
void DisplayAnimation_SetEntry(void *, u32, u32, u32, u32, u32, u32);
void DisplayAnimation_UpdatePlayback(void *);
void DisplayAnimation_SetPriority(void *, s32);

void BestiaryDisplay_CreateEntryAnimations(struct BestiaryDisplay *p)
{
    struct EntryIds ids = gUnk_0824FC6C;
    struct BestiaryAnimation *animations = p->animations;
    const struct BestiaryDisplayConfig *config = &gUnk_086126E0[p->entry];
    s32 i;
    struct BestiaryAnimation *animation;

    if (p->mode == 0) {
        s32 x = config->x;
        s32 y = config->y;
        u16 *origin = p->origin;

        x += 64;
        y += 84;
        origin[0] = x;
        origin[1] = y;
        origin[2] = 0;
        origin[3] = 0;
    }
    p->scale = config->direction;
    Dir_Decompose(config->directionIndex, &p->dirX, &p->dirY, p->scale);
    for (i = 4; i >= 0; i--) {
        animation = &animations[i];
        if (DisplayAnimation_InitializeKeyframes(animation, 0x7516, 16, p->origin) < 0)
            return;
        DisplayAnimation_SetEntry(&p->animations[i], ids.id[i], p->dirX, p->dirY, 1, 0, 5);
        DisplayAnimation_UpdatePlayback(&p->animations[i]);
        DisplayAnimation_SetPriority(animation, 3);
    }
    animation = &animations[5];
    if (DisplayAnimation_InitializeSequence(animation, 0x4147, 0xc5e2, 16, p->origin) < 0)
        return;
    animations[5].position[1] -= 12;
    DisplayAnimation_SetEntry(&p->animations[5], 4, p->dirX, p->dirY, 1, 0, 5);
    DisplayAnimation_UpdatePlayback(&p->animations[5]);
    DisplayAnimation_SetPriority(animation, 2);
}
