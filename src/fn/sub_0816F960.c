#define MotionSequence_UpdateInterpolation PauseMenu_UpdateSpriteMotion
#define MotionSequence_SetCallback Hud_SetInteractionCallback
#include "global.h"
/* Tentative semantic names; field offsets are established by static accesses. */
struct MotionSequence {
    u8 unknown0[0xA4E];
    u16 delay;
    u8 unknownA50[0x2AA8-0xA50];
    u32 flags;
    u8 unknown2AAC[0x438A-0x2AAC];
    u8 selection;
};
u32 PauseMenu_UpdateSpriteMotion(struct MotionSequence *, u32);
void sub_0816D634(struct MotionSequence *, u32);
void SceneHud_UpdateTimedMessage(void);
void Hud_SetInteractionCallback(struct MotionSequence *, void (*)(void), int);
void Sound_PlaySong(u32);
void MotionSequence_AdvanceAfterDelay(struct MotionSequence *p)
{
    if (PauseMenu_UpdateSpriteMotion(p, 0)) {
        if (++p->delay > 7) {
            p->flags |= 1;
            sub_0816D634(p, p->selection);
            Hud_SetInteractionCallback(p, SceneHud_UpdateTimedMessage, 1);
            Sound_PlaySong(0x10D);
        }
    }
}
