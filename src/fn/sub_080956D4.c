#include "global.h"
/* SceneActorBehavior_OnContactForward: forwards the contact unless the actor is unavailable or flagged */
void SceneActorBehavior_ApplyContactReaction(void *, void *);
static inline u8 Unavailable(u8 *c)
{
    u32 mask;
    u32 *p;
    u32 off;
    if (!c)
        goto zero;
    mask = 0x40;
    off = 0xB5;
    off <<= 2;
    p = (u32 *)(c + off);
    if ((*((u16 *)p)) & mask)
        goto one;
    mask = 0x80;
    mask <<= 10;
    off -= 4;
    p = (u32 *)(c + off);
    if ((*p) & mask)
        goto one;
    if ((*((s16 *)(c + 0x9C))) > 0)
        goto zero;
one:
    return 1;
zero:
    return 0;
}
void SceneActorBehavior_OnContactForward(u16 *source, void *destination, u8 *actor)
{
    if (Unavailable(actor))
        return;
    {
        u32 mask = 0x4000;
        u16 *f = (u16 *)(actor + 0x2D4);
        if (*f & mask)
            return;
    }
    {
        u32 mask = 0x800;
        if (source[3] & mask)
            return;
    }
    SceneActorBehavior_ApplyContactReaction(source, destination);
}
