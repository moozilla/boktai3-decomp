#include "global.h"
#include "engine.h"
#include "player.h"
struct ItemDef {
    u16 unknown0;
    u16 actionKind;
    u16 unknown4;
    u16 valueA;
    u16 valueB;
    u8 unknownA[6];
};
extern const struct ItemDef gUnk_08E61280[];
s32 Script_GetActorIndex(void);
s32 Div(s32, s32);
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
/* Apply item E to the player. Timed effects accept t in frames, and healing displays the amount restored. */
void sub_0815A73C(void)
{
    s32 i = Script_GetActorIndex();
    struct Player *p = gPlayers[i];

    if (p != NULL && Script_SeekToKeyword('E')) {
        s32 id = Script_GetValue();
        if (gUnk_08E61280[id].actionKind == 5) {
            s32 timer =
                Script_SeekToKeyword('t') ? Script_GetValue() : gUnk_08E61280[id].valueB * 60;
            p->sceneMode = gUnk_08E61280[id].valueA;
            p->active = timer;
            p->unk536 = id;
            p->actionEighteenParameter = 0x16E;
            p->actionEighteenDuration = 48;
        } else if (gUnk_08E61280[id].actionKind == 1) {
            s32 amount = Div(p->maximumStatA * gUnk_08E61280[id].valueA, 100);
            s32 hp = p->currentStatA + amount;
            if (hp > p->maximumStatA) {
                amount = p->maximumStatA - p->currentStatA;
                p->currentStatA = p->maximumStatA;
            } else {
                p->currentStatA = hp;
            }
            if (amount > 0) {
                DamageNumberDisplay_Show(&p->collider, amount, 1, &p->position,
                                         &p->movementRequestTarget, 0);
            }
            p->actionEighteenParameter = 0x16E;
            p->actionEighteenDuration = 48;
        }
    }
}
