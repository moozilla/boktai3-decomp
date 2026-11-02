#include "player_cue_sequence.h"

// confidence=low: script body initialization installs this auxiliary sprite and animation.
struct Vec {
    s16 x, y, z;
};
void KeyframeActor_InitAuxiliarySprite(u8 *arg)
{
    u8 *p = arg;
    void *data = p + 0x198;
    void *sequence;
    struct ContactSwarmSprite *render;
    u32 zero;
    struct DisplaySequenceState *frames;
    AnimData_Load(data, 0xD495);
    sequence = ScriptProc_Start(0x922E, 0xAF44);
    *(void **)(p + 0x410) = sequence;
    render = (void *)(p + 0x14C);
    *(void **)(p + 0x178) = data;
    zero = 0;
    Sprite_Init(render, data, 0x1001);
    {
        u32 value = *(u16 *)(p + 0x30);
        *(s16 *)(p + 0x168) = value;
        {
            u32 y = *(u16 *)(p + 0x32);
            *(s16 *)(p + 0x16A) = y;
        }
        value = *(u16 *)(p + 0x34);
        *(s16 *)(p + 0x16C) = value;
    }
    *(u16 *)(p + 0x4E8) = zero;
    frames = (void *)(p + 0x3FC);
    Anim_SetSequence(frames, sequence, 0, p[0x3A2], p[0x3A3]);
    PlayerCue_StepSequence(render, frames);
}
