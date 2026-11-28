#include "global.h"
#include "ui/books/bestiary_display.h"

void PaletteTransition_StartDisplay(struct BestiaryDisplay *, u32);
void AnimData_Load(void *, u32);
void Sprite_Init(struct DisplaySequenceSprite *, void *, u32);

static inline void CpuSetArgs(void *dest, u32 control, void *src)
{
    CpuSet(src, dest, control);
}

int sub_081D4CE0(struct BestiaryDisplay *p)
{
    struct DisplaySequenceSprite *sprite;
    u32 fill;

    p->entry = 0;
    p->entryChanged = 0;
    p->hidden = 0;
    if ((u32)p->entry > 0x52)
        p->entry = 0;
    PaletteTransition_StartDisplay(p, p->entry);
    AnimData_Load(p->animData, 0x1c1b);
    sprite = &p->sprite;
    Sprite_Init(sprite, p->animData, 0x212);
    sprite->frame = 5;
    p->sprite.priority = 3;
    p->sprite.position.x = p->origin[0];
    p->sprite.position.y = p->origin[1];
    p->sprite.position.z = p->origin[1];
    p->sprite.palette = 0x80;
    p->sprite.paletteVariant = 0x40;
    p->swayPhase = 0;
    p->mode = 0;
    fill = 0;
    CpuSetArgs(p->origin, 0x05000002, &fill);
    return 0;
}
