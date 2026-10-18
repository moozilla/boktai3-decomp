#include "global.h"

struct S {
    u8 f[0x184];
    u8 a;
};
void DisplayAnimation_Destroy(void *);
void Actor_OnControllerDestroy(struct S *p)
{
    DisplayAnimation_Destroy(&p->a);
}
