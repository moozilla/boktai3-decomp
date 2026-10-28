#include "global.h"
#include "render.h"
#include "input.h"
// Provisional name: Presentation_Update
struct Sequence {
    u8 p0[0x18];
    u8 resource[0x20];
    u8 layer0[8];
    u32 flags0;
    u8 p1[0x98 - 0x44];
    u8 layer1[8];
    u32 flags1;
    u8 p2[0xb8 - 0xa4];
    s16 x1, y1;
    u8 p3[0xf8 - 0xbc];
    u8 layer2[8];
    u32 flags2;
    u8 p4[0x158 - 0x104];
    u8 layer3[8];
    u32 flags3;
    u8 p5[0x178 - 0x164];
    s16 x3, y3;
    u8 p6[0x1b8 - 0x17c];
    u8 layer4[8];
    u32 flags4;
    u8 p7[0x21c - 0x1c4];
    u16 option, state, timer;
    u8 p9[2];
    u16 frame, last, frameTimer, duration, result, exitTimer;
    void *handle;
    void (*callback)(void *);
};
void Anim_LoadKeyframe(void *, void *, s32, s32);
void Anim_SetKeyframes(void *, void *, s32, s32, s32);
void Presentation_UpdateLayerAnimations(void *);
void Presentation_ClearTilemap(void);
void Task_Kill(void *);
void Script_Call(void *, void *);
void Presentation_UpdateDropShakeBlink(void *);
s32 ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void MessageWindow_SetDisplayMode(s32);
void MessageWindow_ShowMessage(s32);
struct Message {
    u32 count : 16;
    u32 unused : 16;
    void *values;
};
// Advance a scripted presentation, handle skipping, and notify its caller on exit.
s32 Presentation_Update(struct Sequence *p)
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
                Anim_SetKeyframes(p->layer2, p->resource, 4, 1, 0);
                Anim_LoadKeyframe(p->layer3, p->resource, 1, 1);
                p->flags4 |= 1;
            } else if (p->option == 3) {
                p->callback = Presentation_UpdateDropShakeBlink;
                p->state = 0;
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
