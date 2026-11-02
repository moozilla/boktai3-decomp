#include "presentation_sequence.h"
#include "render.h"
#include "input.h"
// Provisional name: Presentation_Update
void Anim_LoadKeyframe(void *, void *, s32, s32);
void Anim_SetKeyframes(void *, void *, s32, s32, s32);
void Presentation_UpdateLayerAnimations(void *);
void Presentation_ClearTilemap(void);
void Task_Kill(void *);
void Script_Call(void *, void *);
s32 ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void MessageWindow_SetDisplayMode(s32);
void MessageWindow_ShowMessage(s32);
struct Message {
    u32 count : 16;
    u32 unused : 16;
    void *values;
};
// Advance a scripted presentation, handle skipping, and notify its caller on exit.
s32 Presentation_Update(struct PresentationSequence *p)
{
    s32 value;
    struct Message message;
    if (p->callback)
        p->callback(p);
    if (p->exitTimer) {
        if (++p->exitTimer > 16) {
            if (p->handle) {
                message.count = 1;
                message.values = &value;
                value = p->result;
                Script_Call(p->handle, &message);
            }
            Task_Kill(p);
        }
    } else {
        if (p->frame == p->last && p->frameTimer == p->duration - 16) {
            ScreenTransition_SendCommand(1, 4, 4, 4, 4, 0xffff, 0);
            p->result = 0;
            p->exitTimer = 1;
        } else if (p->timer > 16 && (gPadState[1] & 11)) {
            ScreenTransition_SendCommand(1, 4, 4, 4, 4, 0xffff, 0);
            p->result = 1;
            p->exitTimer = 1;
        } else if (p->frameTimer >= p->duration && p->option) {
            p->frame++;
            if (p->option == 1) {
                Anim_SetKeyframes(&p->layers[2], &p->resource, 4, 1, 0);
                Anim_LoadKeyframe(&p->layers[3], &p->resource, 1, 1);
                p->layers[4].flags |= 1;
            } else if (p->option == 3) {
                p->callback = Presentation_UpdateDropShakeBlink;
                p->dropTimer = 0;
            }
            p->frameTimer = 0;
            MessageWindow_SetDisplayMode(1);
            MessageWindow_ShowMessage(p->frame);
        }
        p->timer++;
        p->frameTimer++;
    }
    if (p->result == 1) {
        u16 *dest = &gBgPaletteBuffer;
        u32 value = 0x1084;
        *dest = value;
    }
}
