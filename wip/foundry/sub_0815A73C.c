#define Player_ApplyScriptRestoration sub_0815A73C
#define Player_GetScriptSlot          sub_0815A400
#define gPlayerRestorationEntries     gUnk_08E61280
#include "global.h"
#include "player.h"
struct PlayerRestorationEntry {
    u16 unknown0, kind, unknown4, value, duration, unknownA[3];
};
extern const struct PlayerRestorationEntry gUnk_08E61280[];
s32 sub_0815A400(void);
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
s32 Div(s32, s32);
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
void sub_0815A73C(void)
{
    struct Player *p;
    const struct PlayerRestorationEntry *entry;
    s32 index, amount, total;
    s32 slot = sub_0815A400();
    p = gUnk_02000580[slot];
    if (!p)
        return;
    if (!Script_SeekToKeyword(0x45))
        return;
    index = Script_GetValue();
    {
        const struct PlayerRestorationEntry *base = gUnk_08E61280;
        entry = (const struct PlayerRestorationEntry *)((index << 4) + (u32)base);
    }
    if (entry->kind == 5) {
        if (Script_SeekToKeyword(0x74))
            amount = Script_GetValue();
        else
            amount = entry->duration * 60;
        p->sceneMode = gUnk_08E61280[index].value;
        p->active = amount;
        p->unk536 = index;
    } else {
        if (entry->kind != 1)
            return;
        amount = Div(p->maximumStatA * entry->value, 100);
        total = p->currentStatA + amount;
        if (total > p->maximumStatA) {
            amount = p->maximumStatA - p->currentStatA;
            p->currentStatA = p->maximumStatA;
        } else
            p->currentStatA = total;
        if (amount > 0)
            DamageNumberDisplay_Show(&p->collider, amount, 1, &p->position,
                                     &p->movementRequestTarget, 0);
    }
    p->actionEighteenParameter = 0x16E;
    p->actionEighteenDuration = 0x30;
}
