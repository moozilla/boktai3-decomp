#include "group_objects.h"
// MATCHING_CONCESSION: register-pinning
int BoardPiece_Initialize(struct GroupObjects *owner, struct GroupPiece *p, int index)
{
    struct GroupResource *resource;
    struct GroupSprite *sprite;
    {
        int zero = 0;
        asm("" : "+r"(zero));
        p->state = zero;
        p->active = 1;
        p->timer = 0;
        p->direction = zero;
    }
    resource = &p->resource;
    AnimData_Load(resource, 0xDA6D);
    {
        void *r = resource;
        asm("" : "+r"(r));
        AnimData_SelectTiles(r, 0x2D3);
    }
    resource->palette = owner->palette;
    sprite = &p->sprite;
    Sprite_Init(sprite, resource, 1);
    sprite->mode = 2;
    {
        register struct GroupVec *position asm("r5") = &p->sprite.position;
        {
            u32 zero = 0;
            {
                register u32 *src asm("r0") = &zero;
                struct GroupVec *dst = position;
                asm("" : : "r"(src), "r"(dst));
                CpuSet(src, dst, 0x05000002);
            }
        }
        Shadow_Initialize(p->transform, position, 0);
    }
    ActorShadow_Disable(p->transform);
}
