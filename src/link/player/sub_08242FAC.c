#include "global.h"
#include "player/player.h"

s32 Div(s32, s32);
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);

void sub_08242FAC(struct Player *p, s32 amount)
{
    if (p->kind == 1) {
        s32 heal = Div(amount * 5 * 4, 100);
        if (heal <= 0)
            heal = 1;
        if (p->currentStatA + heal > p->maximumStatA) {
            heal = p->maximumStatA - p->currentStatA;
            p->currentStatA = p->maximumStatA;
        } else {
            p->currentStatA += heal;
        }
        if (heal > 0)
            DamageNumberDisplay_Show(&p->collider, heal, 1, &p->position, &p->movementRequestTarget,
                                     p->unkAF0);
    }
}
