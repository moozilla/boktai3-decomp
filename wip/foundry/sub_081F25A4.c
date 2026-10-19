#define SpriteTrail_UpdateFlight sub_081F25A4
#define SpriteTrail_UpdatePositions sub_081F2404
#include "global.h"
struct TrailVector { s32 x,y,z,w; };
struct SpriteTrail { u8 entered,state,pad2[2]; struct TrailVector position, history[4], velocity; u8 pad64[0x10]; s32 scroll; u8 pad78[12]; s32 timer; u8 collider[0x40]; u32 spriteFlags; };
void Vector3_CopyComponents(void *,void *);
void Vector3_AddComponents(void *,void *,void *);
void Collider_SetFixedPositionWithYOffset(void *,void *,s32);
void sub_0811B2FC(void *);
void sub_081F2404(void *);
void sub_081F25A4(void *unused, struct SpriteTrail *p)
{
    struct TrailVector delta;
    struct TrailVector *pos;
    struct TrailVector *h;
    void *collider, *sprite;
    s32 i;
    if (p->entered) {
        p->entered=0;
        { u32 *flags=&p->spriteFlags; s32 mask=-2; *flags &= mask; }
    }
    Vector3_CopyComponents(&delta,&p->velocity);
    if (p->timer <= 31) { delta.x >>=3; delta.y >>=3; delta.z >>=3; }
    delta.x += p->scroll;
    pos=&p->position;
    Vector3_AddComponents(pos,pos,&delta);
    sprite=&p->spriteFlags;
    collider=p->collider;
    h=p->history;
    i=3;
    do { h->x += p->scroll; h++; } while (--i>=0);
    Collider_SetFixedPositionWithYOffset(sprite,pos,0);
    Collider_SetFixedPositionWithYOffset(collider,pos,0);
    sub_0811B2FC(collider);
    sub_081F2404(p);
    if(p->timer > 119) { u8 state=2; s32 zero=0; p->state=state; p->entered=1; p->timer=zero; }
    else if (p->position.y>0) { u8 state=1; s32 zero=0; p->state=state; p->entered=state; p->timer=zero; }
    else p->timer++;
}
