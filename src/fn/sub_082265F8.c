#include "global.h"

struct Camera {
    u8 pad0[0x1E];
    u16 field1E;
    u8 pad20[0x76];
    u16 rateIsValue;
    s32 rate;
};
u32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
u8 *Script_GetPc(void);
u8 *Script_DecodeOperand(u8 *, s32 *, s32 *);

/* Shared by the camera move commands (tentative): reads the move rate from
   -e (an operand, kept undecoded) or -R (a value), else 0. */
void Camera_ReadMoveRate(struct Camera *camera)
{
    s32 type, operand;
    u32 found;
    if (camera) {
        camera->field1E = 1;
        if (Script_SeekToKeyword('e')) {
            Script_DecodeOperand(Script_GetPc(), &type, &operand);
            camera->rate = operand;
            camera->rateIsValue = 0;
        } else if ((found = Script_SeekToKeyword('R'))) {
            camera->rate = Script_GetValue();
            camera->rateIsValue = 1;
        } else
            camera->rate = found;
    }
}
