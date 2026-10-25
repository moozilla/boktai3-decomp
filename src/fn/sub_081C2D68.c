#include "global.h"


struct SparkleSprite {
    u32 flags;
    u8 unk_04[0x34];
};

struct SparkleEmitter {
    u8 unk_00[0x18];
    struct SparkleSprite sprites[32];
    u8 animation[0x1C];
    u8 receiver[0x34];
    s32 palette;
    void *script;
    s32 timer;
    s16 angle;
    s16 speed;
    s32 unk_778;
    void (*update)(struct SparkleEmitter *);
};

void AnimData_Load(void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Sprite_Init(void *, void *, u32);
void SceneMessages_RegisterReceiver(void *, void *, u32);
s32 Script_GetKeywordValue(u32, s32);
void Sound_PlaySong(u32);
void Sound_PauseChannel12(u32);
void SparkleEmitter_UpdateEmitting(struct SparkleEmitter *);
void SparkleEmitter_UpdateDraining(struct SparkleEmitter *);

static inline void SparkleEmitter_SetUpdate(struct SparkleEmitter *p,
                                            void (*update)(struct SparkleEmitter *))
{
    p->update = update;
}

static inline s32 SparkleEmitter_SetTimer(struct SparkleEmitter *p, s32 timer)
{
    return p->timer = timer;
}

static inline u8 SparkleEmitter_QuantizeDirection(s16 direction)
{
    s16 phase = direction + 5;
    return (u32)phase << 5;
}

static inline void SparkleEmitter_Start(struct SparkleEmitter *p, s32 speed, s32 angle, u32 palette)
{
    p->speed = speed;
    p->angle = angle;
    p->palette = palette;
    if (palette <= 14)
        AnimData_SelectPaletteBank(p->animation, palette + 0x255);
    Sound_PlaySong(0x83);
    SparkleEmitter_SetUpdate(p, SparkleEmitter_UpdateEmitting);
    SparkleEmitter_SetTimer(p, 0);
}

s32 SparkleEmitter_Initialize(struct SparkleEmitter *p, void *script)
{
    void *animation;
    struct SparkleSprite *sprite;
    s32 i;
    u32 one;

    p->script = script;
    animation = p->animation;
    AnimData_Load(animation, 0xCC88);
    AnimData_SelectPaletteBank(animation, 0x255);
    sprite = p->sprites;
    i = 0;
    one = 1;
    do {
        u32 flags;
        Sprite_Init(sprite, p->animation, 0);
        flags = sprite->flags | one;
        sprite->flags = flags;
        if (i & one)
            flags |= 0x200;
        else
            flags |= 0x400;
        sprite->flags = flags;
        sprite++;
        i++;
    } while (i <= 31);
    SceneMessages_RegisterReceiver(p->receiver, p->script, 10);
    SparkleEmitter_SetUpdate(p, SparkleEmitter_UpdateEmitting);
    SparkleEmitter_SetTimer(p, 0);
    p->unk_778 = 0;
    p->angle = SparkleEmitter_QuantizeDirection(Script_GetKeywordValue('d', 5));
    p->speed = Script_GetKeywordValue('p', 64);
    p->palette = Script_GetKeywordValue('c', 8);
    if (Script_GetKeywordValue('i', 1)) {
        SparkleEmitter_Start(p, p->speed, p->angle, p->palette);
    } else {
        Sound_PauseChannel12(8);
        SparkleEmitter_SetUpdate(p, SparkleEmitter_UpdateDraining);
        SparkleEmitter_SetTimer(p, 0);
    }
    return 0;
}
