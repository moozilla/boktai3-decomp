#include "global.h"
#include "ram.h"
struct PanelPosition {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));

struct PanelDisplay {
    u8 unknown0[0x10];
    s16 sort;
    u8 unknown12[0xA];
    struct PanelPosition position;
    u8 unknown24[8];
    u8 graphics[0x1E];
    s16 variant;
};
struct Panel {
    u8 unknown0[0x20];
    struct PanelDisplay display[3];
    u8 unknown104[0x578 - 0x104];
    u16 start, cooldown, unknown57C, timer;
    u8 unknown580[8];
    u16 sustainedInput, thresholds[3], multiplier;
    u8 unknown592[2];
    u32 ticks, tickLimit, total, totalLimit, multiplierScript, limitScript, endScript;
    u16 accumulator, duration;
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
s32 ControlPanel_InitializeFromScript(struct Panel *p)
{
    void *anim;
    struct PanelDisplay *sprite;
    void *middleGraphics;
    struct PanelDisplay *middleSprite;
    void *lastGraphics;
    struct PanelDisplay *lastSprite;
    s32 zero;
    s32 value = Script_SeekToKeyword('p');
    if (value)
        p->start = Script_GetValue();
    else
        p->start = value;
    {
        s32 firstZero;
        anim = p->display[0].graphics;
        AnimData_Load(anim, 0xC046);
        sprite = &p->display[0];
        Sprite_Init(sprite, anim, 0);
        firstZero = 0;
        sprite->sort = firstZero;
        p->display[0].position.x = 0x380;
        p->display[0].position.y = firstZero;
        p->display[0].position.z = 0x52C;
        middleGraphics = p->display[1].graphics;
        AnimData_Load(middleGraphics, 0xC046);
        middleSprite = &p->display[1];
        Sprite_Init(middleSprite, middleGraphics, 0);
        middleSprite->sort = firstZero;
        p->display[1].position = p->display[0].position;
        p->display[1].position.z += 0x300;
    }
    if (Script_SeekToKeyword('N')) {
        switch (Script_GetValue()) {
            case 4:
                p->display[1].variant = 1;
                break;
            case 3:
                p->display[1].variant = 4;
                break;
            case 0:
            default:
                p->display[1].variant = 2;
                break;
        }
    } else
        p->display[1].variant = 2;
    lastGraphics = p->display[2].graphics;
    AnimData_Load(lastGraphics, 0xC046);
    lastSprite = &p->display[2];
    Sprite_Init(lastSprite, lastGraphics, 0);
    lastSprite->sort = 0;
    p->display[2].position = p->display[0].position;
    p->display[2].position.z += -0x300;
    sub_081C2334(p);
    ControlPanel_ShowRewardBalanceDigits(p);
    ControlPanel_SetUpdate(p, ControlPanel_UpdateIdle);
    sub_081C24D0(p);
    IconRow_Init(p);
    ControlPanel_HideInteractionDisplay(p);
    if (Script_SeekToKeyword('r'))
        p->duration = Script_GetValue();
    else
        p->duration = 300;
    if (Script_SeekToKeyword('t')) {
        p->thresholds[0] = Script_GetValue();
        p->thresholds[1] = Script_GetValue();
        p->thresholds[2] = Script_GetValue();
    } else {
        p->thresholds[0] = 180;
        p->thresholds[1] = 300;
        p->thresholds[2] = 600;
    }
    value = Script_SeekToKeyword('b');
    if (value)
        p->multiplierScript = Script_GetValue();
    else
        p->multiplierScript = value;
    {
        u16 *flag = &p->multiplier;
        zero = 0;
        *flag = 1;
    }
    p->sustainedInput = zero;
    if (Script_SeekToKeyword('e'))
        p->endScript = Script_GetValue();
    if (Script_SeekToKeyword('a')) {
        p->tickLimit = Script_GetValue();
        p->totalLimit = Script_GetValue();
        p->limitScript = Script_GetValue();
    }
    {
        u32 *counter = &p->ticks;
        *counter = zero;
        p->total = zero;
        Sound_PlaySong(0x23E);
        p->timer = zero;
        *counter = zero;
    }
    gSegmentDisplay = (void *)p;
    return 0;
}
