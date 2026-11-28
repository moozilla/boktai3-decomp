#include "global.h"
#include "engine/video/render.h"
#include "engine/video/anim_sprite.h"
#include "engine/video/sprite_drawable_api.h"
#include "minigames/result_game/result_tutorial.h"
extern u8 gUnk_0824DB28[];
s32 Anim_LoadKeyframe(void *anim, void *data, u32 index, u32 mode);
void Anim_SetKeyframes(void *anim, void *data, u32 index, u32 mode, u32 flags);
void *AnimData_FindEntry(s32 id);

static inline void Clamp(s16 *v, s32 lo, s32 hi)
{
    if (*v < lo)
        *v = lo;
    else if (*v > hi)
        *v = hi;
}
s32 ResultGame_InitializeDisplay(struct ResultMenu *menu)
{
    u32 layer = menu->layer;
    s16 frame;
    u32 layer1 = layer + 1;
    s32 halfWidth;
    u8 *halfWidths = gUnk_0824DB28 + 5;
    struct AnimSprite *sprite;
    const struct SpriteEffectTileResource *entry;
    s32 i;
    s32 y;

    switch (halfWidths[menu->difficulty]) {
        case 10:
            halfWidth = 10;
            frame = 0xbe;
            break;
        case 20:
            halfWidth = 20;
            frame = 0xbf;
            break;
        case 30:
            halfWidth = 30;
            frame = 0xc0;
            break;
        default:
            halfWidth = 40;
            frame = 0xc1;
            break;
    }
    menu->countdown = 0xe10;
    menu->targetCenter = 100;
    menu->targetLeft = menu->targetCenter - halfWidth;
    menu->targetRight = menu->targetCenter + halfWidth;
    Clamp(&menu->targetLeft, 0, 0xb0);
    Clamp(&menu->targetRight, 0, 0xb0);
    menu->lastPressScore = 0;

    sprite = &menu->widgets[0];
    sprite->position.x = 0;
    Anim_LoadKeyframe(sprite, &menu->primaryResource, 0xb4, 0);
    sprite->flags &= ~1;
    sprite->attribute1A = layer;

    sprite = &menu->widgets[1];
    sprite->position.x = 0;
    Anim_LoadKeyframe(sprite, &menu->primaryResource, 0xbd, 0);
    sprite->attribute1A = layer;
    sprite->flags |= 1;

    sprite = &menu->widgets[2];
    sprite->position.x = 0;
    Anim_LoadKeyframe(sprite, &menu->primaryResource, 0xbc, 0);
    sprite->flags |= 1;
    sprite->attribute1A = layer;

    sprite = &menu->widgets[3];
    sprite->position.x = menu->targetCenter;
    Anim_LoadKeyframe(sprite, &menu->primaryResource, frame, 0);
    sprite->flags |= 1;

    sprite = &menu->widgets[4];
    sprite->position.x = menu->lastPressScore - 0xb0;
    Anim_LoadKeyframe(sprite, &menu->primaryResource, 0xb8, 0);
    sprite->attribute1A = layer1;
    sprite->flags &= ~1;
    CpuSet(gPaletteBankData + 0x75a0, menu->meterPalette, 0x04000008);
    sprite->paletteBank = 0x8100;
    sprite->palette = menu->meterPalette - 48; /* three banks below the meter colors */

    sprite = &menu->widgets[5];
    sprite->position.x = 0;
    Anim_SetKeyframes(sprite, &menu->primaryResource, 1, 1, 0);
    sprite->flags &= ~1;

    sprite = &menu->widgets[6];
    y = 0x20;
    for (i = 0; i < 5; i++, sprite++, y += 8) {
        sprite->position.x = 8;
        sprite->position.y = y;
        Anim_LoadKeyframe(sprite, &menu->primaryResource, 0xc4, 0);
        sprite->attribute1A = 0;
        sprite->flags |= 1;
    }
    menu->cameraPosition = menu->initialCameraPosition;

    sprite = &menu->widgets[11];
    Anim_LoadKeyframe(sprite, &menu->primaryResource, 0xb6, 0);
    sprite->flags &= ~1;
    sprite->attribute1A = layer1;

    sprite = &menu->widgets[12];
    Anim_LoadKeyframe(sprite, &menu->primaryResource, 0xce, 0);
    sprite->flags |= 1;
    sprite->attribute1A = layer;

    sprite = &menu->widgets[13];
    Anim_LoadKeyframe(sprite, &menu->primaryResource, 0xb7, 0);
    sprite->flags &= ~1;
    sprite->attribute1A = layer;

    entry = AnimData_FindEntry(0x1c1a);
    for (i = 0; i < 4; i++) {
        struct SpriteDrawable *q = &menu->markers[i];
        Effect_SetOrigin(q, -8, -8);
        SpriteDrawable_SelectTileFrame(q, entry, 0x33);
        q->priority = 0;
        ZeroFill8(&q->position);
    }
    menu->markers[0].position.x = 0x59;
    menu->markers[0].position.y = 0x78;
    menu->markers[0].position.z = 0;

    sprite = &menu->widgets[14];
    sprite->position.x = -8;
    sprite->position.y = -0x48;
    sprite->position.z = 0;
    Anim_LoadKeyframe(sprite, &menu->primaryResource, 0x3d, 0);
    sprite->flags |= 1;
    Anim_SetKeyframes(sprite, &menu->primaryResource, 0, 1, 0);
    sprite->attribute1A = 0;
    return 0;
}
