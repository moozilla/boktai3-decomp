#include "player/effects/moving_glyph.h"
#include "global.h"
#include "engine/engine.h"
#include "scene/terrain/terrain_edge_marker.h"

void MovingGlyph_SetState(struct MovingSprite *p, void (*phase)(void))
{
    p->phase = phase;
    p->frame = 0;
}

static inline u32 Flip(u16 kind)
{
    u32 flip = 0;
    if (kind > 2) {
        flip = 8;
        if (kind > 4) {
            flip = 4;
            if (kind <= 5)
                flip = 0xC;
        }
    }
    return flip;
}

// confidence=low: three frames from baseFrame, then three of a frame and flip picked by kind;

// kills itself after six frames.

void MovingGlyph_UpdateFlash(struct MovingSprite *p)
{
    if (p->frame <= 2)
        p->sprite.frame = (p->id >> 1) + 4;
    else {
        s32 offset;
        if (p->angle & 1)
            offset = 1;
        else
            offset = (p->angle >> 1) & 1 ? 2 : 0;
        p->sprite.flags |= Flip(p->angle);
        p->sprite.frame = offset + 6;
    }
    if (++p->frame > 5)
        Task_Kill(p);
}

void MovingSprite_UpdateMotion(struct MovingSprite *p)
{
    p->position.x += p->velocityX;
    p->position.z += p->velocityY;
    p->sprite.position = p->position;
    p->frame++;
    if (p->frame >= p->duration)
        MovingGlyph_SetState(p, MovingGlyph_UpdateFlash);
}

void MovingSprite_UpdateDelay(struct MovingSprite *p)
{
    p->frame++;
    if (p->frame >= p->delay) {
        p->sprite.flags &= ~1;
        MovingGlyph_SetState(p, MovingSprite_UpdateMotion);
    }
}

u32 MovingGlyph_UpdateTask(struct MovingSprite *p)
{
    Callback_InvokeActor(p, p->phase);
    return 0;
}

u32 MovingGlyph_DestroyTask(struct MovingSprite *p)
{
    Sprite_Remove(&p->sprite);
    return 0;
}

static inline s32 scale(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}

s32 MovingSprite_Initialize(struct MovingSprite *p, const struct ContactPosition *position,
                            u32 palette, u32 id, u32 angle, u32 speed, u32 delay, u32 duration)
{
    struct DisplaySequenceSprite *sprite = &p->sprite;
    u32 frame;
    s32 index;

    AnimData_Load(p->animation, 0x210E);
    Sprite_Init(sprite, p->animation, 0);
    p->position = *position;
    sprite->position = *position;
    sprite->priority = 1;
    AnimData_SelectPaletteBank(p->animation, palette);
    p->id = id;
    frame = p->id;
    sprite->frame = frame;
    p->angle = angle;
    index = ((p->angle + 1) & 7) << 5;
    p->velocityX = -scale(gUnk_086149C4[(index + 64) & 255] * speed);
    p->velocityY = -scale(gUnk_086149C4[index] * speed);
    p->delay = delay;
    p->duration = duration;
    if (p->delay) {
        sprite->flags |= 1;
        MovingGlyph_SetState(p, MovingSprite_UpdateDelay);
    } else {
        MovingGlyph_SetState(p, MovingSprite_UpdateMotion);
    }
    return 0;
}

void *MovingGlyph_Create(void *position, u32 id, u32 a, u32 direction, u32 duration, u32 delay,
                         u32 b)
{
    void *p = Task_Create(10, 0x7C);
    if (p) {
        Task_SetCallbacks(p, MovingGlyph_UpdateTask, MovingGlyph_DestroyTask);
        if (MovingSprite_Initialize(p, position, id, a, direction, duration, delay, b) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

s32 MovingGlyph_InitializeFromScript(void *p, u32 unusedA, u32 unusedB)
{
    union TerrainMarkerPosition position;
    s32 palette, id, angle, speed, delay, duration;
    if (Script_SeekToKeyword('p')) {
        position.packed.x = Script_GetValue();
        position.packed.y = Script_GetValue();
        position.packed.z = Script_GetValue();
    } else {
        u32 mask = 0xFFFF0000;
        position.words[0] = 0;
        position.words[1] &= mask;
    }
    if (Script_SeekToKeyword('c'))
        palette = Script_GetValue() + 0x4B;
    else
        palette = 0x4B;
    if (Script_SeekToKeyword('E'))
        id = Script_GetValue();
    else
        id = 0;
    if (Script_SeekToKeyword('d'))
        angle = Script_GetValue();
    else
        angle = 0;
    if (Script_SeekToKeyword('s'))
        speed = Script_GetValue();
    else
        speed = 0x20;
    if (Script_SeekToKeyword('w'))
        delay = Script_GetValue();
    else
        delay = 0;
    if (Script_SeekToKeyword('l'))
        duration = Script_GetValue();
    else
        duration = 0x5A;
    MovingSprite_Initialize(p, &position, palette, id, angle, speed, delay, duration);
    return 0;
}

void *MovingGlyph_CreateFromScript(u32 a, u32 b)
{
    void *task = Task_Create(9, 0x7c);
    if (task != NULL) {
        Task_SetCallbacks(task, MovingGlyph_UpdateTask, MovingGlyph_DestroyTask);
        if (MovingGlyph_InitializeFromScript(task, a, b) < 0) {
            Task_Kill(task);
            return NULL;
        }
    }
    return task;
}
