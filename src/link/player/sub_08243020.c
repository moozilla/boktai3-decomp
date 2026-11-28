#include "global.h"
#include "player/player.h"

s32 Div(s32, s32);
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);

void sub_08243020(struct Player *p, s32 amount)
{
    if (p->kind == 1) {
        s32 heal = Div(amount * 5, 100);
        if (heal <= 0)
            heal = 1;
        if (p->currentStatB + heal > p->maximumStatB) {
            heal = p->maximumStatB - p->currentStatB;
            p->currentStatB = p->maximumStatB;
        } else {
            p->currentStatB += heal;
        }
        if (heal > 0)
            DamageNumberDisplay_Show(&p->collider, heal, 1, &p->position, &p->movementRequestTarget,
                                     p->unkAF0);
    }
}
