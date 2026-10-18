#define Player_BeginDescriptorAction sub_0813EABC
#define Player_SetActionEntry Player_SetWeaponInstance
#define gPlayerActionDescriptors     gUnk_08E6092C
#include "global.h"
#include "player.h"
#include "scene_input_compare.h"
extern const struct PlayerActionDescriptor gUnk_08E6092C[];
void Player_SetWeaponInstance(struct Player *, u32);
void sub_0822D7C0(void *);
void sub_0822D764(void *, void *);
void sub_0813E608(struct Player *, struct PlayerStatModifierContext *, const void *);
void sub_0813E90C(struct Player *);
void sub_0814F328(void);
void sub_0814D9C4(void);
void sub_0813AFEC(struct Player *, s32, s32);
void sub_08160550(void *, u32);
void sub_0813EABC(struct Player *p, u32 entry)
{
    struct PlayerActionDescriptor descriptor;
    if (p->actionMode != 6) {
        Player_SetWeaponInstance(p, entry);
        if (p->statModifierContext.actionEntry == NULL)
            sub_0822D7C0(&descriptor);
        else
            sub_0822D764(p->statModifierContext.actionEntry, &descriptor);
    } else {
        p->statModifierContext.actionEntry = NULL;
        descriptor = gUnk_08E6092C[47];
    }
    p->statModifierContext.actionKind = descriptor.kind;
    p->statModifierContext.statBOverrideEnabled = 0;
    p->statModifierContext.actionDuration = descriptor.duration;
    p->statModifierContext.actionPhase =
        Player_GetEquipmentValue((const struct SceneInputItemDescriptor *)&descriptor);
    sub_0813E608(p, &p->statModifierContext, &descriptor);
    if (p->statModifierContext.actionKind == 0x30)
        p->actionCallback = sub_0814F328;
    else
        p->actionCallback = sub_0814D9C4;
    p->actionResetCounter = 0;
    sub_0813E90C(p);
    if (p->tetherMode == 3)
        sub_0813AFEC(p, 0, 0);
    if (p->statModifierContext.actionKind != 0x30)
        sub_08160550(p->actionDisplay,
                     gUnk_08E6092C[p->statModifierContext.actionKind].displaySelection);
}
