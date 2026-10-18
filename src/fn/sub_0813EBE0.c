#define Player_BeginIndexedAction sub_0813EBE0
#define gPlayerIndexedActions     gUnk_08E60F4C
#include "global.h"
#include "player.h"
extern const struct PlayerIndexedAction gUnk_08E60F4C[];
void sub_0814A9D4(void);
void sub_0814EEE0(void);
void sub_0814E990(void);
void sub_0814E3D8(void);
void sub_0813E90C(struct Player *);
void sub_0813AFEC(struct Player *, s32, s32);
static inline void ClearFlag(struct Player *p, u32 index)
{
    if (!p)
        return;
    if (index > 63)
        p->actionStateFlags &= ~(1 << (index - 64));
    else if (index > 31)
        p->actionFlags &= ~(1 << (index - 32));
    else
        p->statBonusFlags &= ~(1 << index);
}
void sub_0813EBE0(struct Player *p, s32 action)
{
    s32 index = action - 1;
    const struct PlayerIndexedAction *table = gUnk_08E60F4C;
    const struct PlayerIndexedAction *descriptor;
    s32 i;
    if (p) {
        for (i = 0; i <= 6; i++)
            ClearFlag(p, i);
    }
    if (p->actionMode != 6)
        descriptor = &table[index];
    else
        descriptor = &table[index];
    p->statModifierContext.actionKind = action;
    p->statModifierContext.actionDuration = descriptor->duration;
    p->statModifierContext.statBOverrideEnabled = 4;
    if (p->actionMode == 6)
        p->actionCallback = sub_0814A9D4;
    else if (p->statModifierContext.actionKind == 11)
        p->actionCallback = sub_0814EEE0;
    else if (p->statModifierContext.actionKind == 8)
        p->actionCallback = sub_0814E990;
    else
        p->actionCallback = sub_0814E3D8;
    sub_0813E90C(p);
    if (p->tetherMode == 3)
        sub_0813AFEC(p, 0, 0);
}
