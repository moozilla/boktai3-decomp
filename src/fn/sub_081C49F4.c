#include "global.h"
#include "display_animation.h"
#include "render.h"
#include "player_cue_sequence.h"
struct WheelSprite {
    u32 flags;
    u8 pad4[0xC];
    u16 frame;
    u8 pad12[0xA];
    s16 x, y, z;
    u8 pad22[0xA];
};
struct Wheel {
    u8 task[0x18];
    struct WheelSprite sprite;
    u8 resource[6];
    u16 paletteIndex;
    u8 resource8[4];
    void *paletteData;
    u8 resource10[0xC];
    struct DisplaySequenceState animation;
    void *animationData;
    u8 receiver[0x34];
    void *owner;
    u8 palette[0x18];
    u8 paletteBuffer[0x20];
};
void TimePalette_InitTransition(void *, void *, void *, void *);
void SceneMessages_RegisterReceiver(void *, void *, s32);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);

inline static u32 Arg1(u32 k_, u8 *resource)
{
    return resource + k_;
}

s32 sub_081C49F4(struct Wheel *p, void *owner)
{
    struct WheelSprite *sprite = &p->sprite;
    u8 *resource = p->resource;
    AnimData_Load(resource, 0x2173);
    TimePalette_InitTransition(p->palette, gPaletteBankData + 0x2D20, gPaletteBankData + 0x2D40,
                               gPaletteBankData + 0x2D60);
    *((void **)Arg1(0xC, resource)) = p->paletteBuffer;
    p->paletteIndex = 0x169;
    Sprite_Init(sprite, resource, 0);
    p->animationData = ScriptProc_Start(0x922E, 0x2990);
    if (!p->animationData)
        return -1;
    Anim_SetSequence(&p->animation, p->animationData, 0, 0, 0);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)sprite, &p->animation);
    if (Script_SeekToKeyword('p')) {
        sprite->x = Script_GetValue();
        sprite->y = Script_GetValue();
        sprite->z = Script_GetValue();
        p->owner = owner;
        SceneMessages_RegisterReceiver(p->receiver, owner, 10);
        return 0;
    }
    return -1;
}
