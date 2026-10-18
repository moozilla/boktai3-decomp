#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Object {
    u8 pad[0x218];
    u8 kind;
    u8 pad219[0x2b];
    u8 effect[1];
};
void Shadow_Initialize(void *, int, int, int);
void ActorShadow_Enable(void *);
void ScaledShadow_Initialize(void *, int, int, int, int, int, int, int, int, int, int);
void ScaledShadow_SetGraphic(void *, int, int);
void ScaledShadow_Enable(void *);
int Actor_InitShadow(struct Object *p, int resource, int kind, int param, int value)
{
    p->kind = kind;
    {
        register unsigned shifted asm("r2") = (unsigned)kind << 24;
        kind = shifted >> 24;
    }
    if (kind == 1) {
        void *e = p->effect;
        Shadow_Initialize(e, resource, 1, value);
        ActorShadow_Enable(e);
    } else if (kind == 2) {
        void *e = p->effect;
        ScaledShadow_Initialize(e, resource, value, value, value, value, 2000, 400, 2, 4, 1);
        ScaledShadow_SetGraphic(e, 0x1c1b, param);
        ScaledShadow_Enable(e);
    }
    return 0;
}
