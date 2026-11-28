#include "npc/manager/scene_actor_lifecycle.h"
void DisplayAnimation_SetFlags(void *, u32);
void DisplayAnimation_ClearFlags(void *, u32);
s32 DisplayAnimation_SetPalette(void *, u32, u32, u32);
void Sound_PlaySong(u32);
s32 __divsi3(s32, s32);
void sub_081FED40(s32);
static inline s32 Interpolate(s32 start, s32 end, s32 tick, s32 duration)
{
    s32 delta;
    if (tick <= 0)
        return start;
    if (tick >= duration)
        return end;
    delta = __divsi3((s32)((u32)(end - start) << 8) * tick, duration);
    if (delta >= 0)
        delta >>= 8;
    else
        delta = -(-delta >> 8);
    return start + delta;
}
void StoryCharacter_RunPaletteFlashSequence(u32 a, struct SceneActor *p, s32 a2)
{
    s32 *effects = (s32 *)p->unknown20;
    SceneActor_SetFlags(p, 0x400);
    if (p->animationEntered != 0) {
        u32 animId;
        p->animationEntered = 0;
        p->finished = 0;
        DisplayAnimation_SetFlags(&p->drawable.common, 2);
        switch (p->variant) {
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 24:
            case 25:
            case 26:
            case 28:
            case 29:
                animId = 0;
                break;
            case 27:
                animId = 0x10;
                break;
            default:
                animId = 0;
                break;
        }
        Actor_PlayAnimationEntry(p, animId, 1, 1, 4);
        p->animationPhase = 0;
        p->stateFrame = 0;
        a2 = 0;
        Sound_PlaySong(0x577);
    }
    switch (p->animationPhase) {
        case 0:
            if (a2 <= 119) {
                s32 color = Interpolate(0x40, 0, a2, 0x78);
                s32 base;
                DisplayAnimation_SetPalette(&p->drawable.common, color, color, 0);
                base = p->target.y;
                p->body.position.y = Interpolate(base, base + 0x40, a2, 0x78);
            } else {
                DisplayAnimation_SetPalette(&p->drawable.common, 0x40, 0x40, 0);
                DisplayAnimation_ClearFlags(&p->drawable.common, 2);
                sub_081FED40(effects[1]);
                p->animationPhase++;
                SceneActor_SetFlags(p, 0x100);
            }
            break;
        case 1:
            SceneActor_SetFlags(p, 0x100);
            p->finished = 1;
            break;
    }
}
