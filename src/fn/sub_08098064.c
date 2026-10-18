#define SceneActor_InitLinkedSprites sub_08098064
#include "global.h"
#include "scene_actor_reset.h"
void *ScriptProc_Start(u16, u16);
struct LinkedReactionVisual {
    struct DisplaySequenceSprite sprite;
    u8 animation[0x2C];
    void *script;
    u16 elapsed, unknown5E;
};
struct LinkedReactionVisualContext {
    u8 unknown0[0x3D4];
    struct LinkedReactionVisual visuals[4];
    u16 elapsed, phase;
};
static inline void ClearElapsed(u8 *animation, u16 value)
{
    *(u16 *)(animation + 0x30) = value;
}
void sub_08098064(struct SceneActorRegistryResetView *actor)
{
    struct LinkedReactionVisualContext *state = actor->context;
    u8 *animation = state->visuals[0].animation;
    struct DisplaySequenceSprite *sprite = &state->visuals[0].sprite;
    s32 i;
    for (i = 0; i < 4; i++) {
        void *script;
        AnimData_Load(animation, 0xD637);
        Sprite_Init(sprite, animation, 0);
        AnimData_SelectPaletteBank(animation, 0x1EE);
        script = ScriptProc_Start(0x922E, 0x29FB);
        state->visuals[i].script = script;
        sprite->flags = 0x201;
        ClearElapsed(animation, 0);
        animation += 0x60;
        sprite = (void *)((u8 *)sprite + 0x60);
    }
    state->elapsed = 0;
    state->phase = 2;
}
