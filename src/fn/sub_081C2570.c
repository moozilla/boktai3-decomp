// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
struct VecWords {
    u32 a, b;
};
struct Panel {
    u8 pad[0x3C];
    s16 x, y, z;
    u8 pad42[0x588 - 0x42];
    s16 unknown588, thresholds[3], unknown590;
    u8 pad592[0x5B2 - 0x592];
    s16 duration;
};
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void sub_081C2334(void *);
void ControlPanel_ShowRewardBalanceDigits(void *);
void ControlPanel_UpdateIdle(void);
void ControlPanel_SetUpdate(void *, void (*)(void));
void sub_081C24D0(void *);
void IconRow_Init(void *);
void ControlPanel_HideInteractionDisplay(void *);
void Sound_PlaySong(u32);
s32 ControlPanel_InitializeFromScript(u8 *input)
{
    u8 *p = input;
    register u8 *anim asm("r5");
    register u8 *sprite asm("r4");
    s32 zero;
    s32 value = Script_SeekToKeyword('p');
    if (value)
        *(s16 *)(p + 0x578) = Script_GetValue();
    else
        *(s16 *)(p + 0x578) = value;
    {
        s32 firstZero;
        anim = p + 0x4C;
        AnimData_Load(anim, 0xC046);
        sprite = p + 0x20;
        Sprite_Init(sprite, anim, 0);
        firstZero = 0;
        *(s16 *)(sprite + 0x10) = firstZero;
        ((struct Panel *)p)->x = 0x380;
        ((struct Panel *)p)->y = firstZero;
        ((struct Panel *)p)->z = 0x52C;
        anim += 0x4C;
        AnimData_Load(anim, 0xC046);
        sprite += 0x4C;
        Sprite_Init(sprite, anim, 0);
        *(s16 *)(sprite + 0x10) = firstZero;
        *(struct VecWords *)(p + 0x88) = *(struct VecWords *)(p + 0x3C);
        *(s16 *)(p + 0x8C) += 0x300;
    }
    if (Script_SeekToKeyword('N')) {
        switch (Script_GetValue()) {
            case 4:
                *(s16 *)(p + 0xB6) = 1;
                break;
            case 3:
                *(s16 *)(p + 0xB6) = 4;
                break;
            case 0:
            default:
                *(s16 *)(p + 0xB6) = 2;
                break;
        }
    } else
        *(s16 *)(p + 0xB6) = 2;
    anim = p + 0xE4;
    AnimData_Load(anim, 0xC046);
    sprite = p + 0xB8;
    Sprite_Init(sprite, anim, 0);
    *(s16 *)(sprite + 0x10) = 0;
    *(struct VecWords *)(p + 0xD4) = *(struct VecWords *)(p + 0x3C);
    *(s16 *)(p + 0xD8) += -0x300;
    sub_081C2334(p);
    ControlPanel_ShowRewardBalanceDigits(p);
    ControlPanel_SetUpdate(p, ControlPanel_UpdateIdle);
    sub_081C24D0(p);
    IconRow_Init(p);
    ControlPanel_HideInteractionDisplay(p);
    if (Script_SeekToKeyword('r'))
        ((struct Panel *)p)->duration = Script_GetValue();
    else
        ((struct Panel *)p)->duration = 300;
    if (Script_SeekToKeyword('t')) {
        ((struct Panel *)p)->thresholds[0] = Script_GetValue();
        ((struct Panel *)p)->thresholds[1] = Script_GetValue();
        ((struct Panel *)p)->thresholds[2] = Script_GetValue();
    } else {
        ((struct Panel *)p)->thresholds[0] = 180;
        ((struct Panel *)p)->thresholds[1] = 300;
        ((struct Panel *)p)->thresholds[2] = 600;
    }
    value = Script_SeekToKeyword('b');
    if (value)
        *(u32 *)(p + 0x5A4) = Script_GetValue();
    else
        *(u32 *)(p + 0x5A4) = value;
    {
        s16 *flag = (s16 *)(p + 0x590);
        zero = 0;
        *flag = 1;
    }
    *(s16 *)(p + 0x588) = zero;
    if (Script_SeekToKeyword('e'))
        *(u32 *)(p + 0x5AC) = Script_GetValue();
    if (Script_SeekToKeyword('a')) {
        *(u32 *)(p + 0x598) = Script_GetValue();
        *(u32 *)(p + 0x5A0) = Script_GetValue();
        *(u32 *)(p + 0x5A8) = Script_GetValue();
    }
    {
        u32 *counter = (u32 *)(p + 0x594);
        *counter = zero;
        *(u32 *)(p + 0x59C) = zero;
        Sound_PlaySong(0x23E);
        *(s16 *)(p + 0x57E) = zero;
        *counter = zero;
    }
    gSegmentDisplay = p;
    return 0;
}
