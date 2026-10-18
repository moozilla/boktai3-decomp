#include "global.h"

struct S {
    u8 f[0x1a3];
    u8 a;
};
void GridAttackActor_ResetAttackAnimationHistory(struct S *p)
{
    p->a = 0;
}
