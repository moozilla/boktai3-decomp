#include "global.h"
// MATCHING_CONCESSION: register-pinning
typedef struct {
    u8 active, state;
    u16 pad2, timer;
    s8 direction;
    u8 pad7[0x39];
    u8 sprite[28];
    u32 position[2];
    u8 pad64[8];
    u8 resource[12];
    void *palette;
    u8 pad7c[12];
    u8 transform[1];
} Piece;
void AnimData_Load(void *, int);
void AnimData_SelectTiles(void *, int);
void Sprite_Init(void *, void *, int);
void Shadow_Initialize(void *, void *, int);
int ActorShadow_Disable(void *);
int BoardPiece_Initialize(u8 *owner, Piece *p)
{
    struct Resource {
        u8 pad[12];
        void *palette;
        u8 tail[12];
    } *resource;
    struct Sprite {
        u8 pad[7], mode;
        u8 tail[20];
    } *sprite;
    {
        int zero = 0;
        asm("" : "+r"(zero));
        p->state = zero;
        p->active = 1;
        p->timer = 0;
        p->direction = zero;
    }
    resource = (void *)p->resource;
    AnimData_Load(resource, 0xDA6D);
    {
        void *r = resource;
        asm("" : "+r"(r));
        AnimData_SelectTiles(r, 0x2D3);
    }
    resource->palette = owner + 0xCA8;
    sprite = (void *)p->sprite;
    Sprite_Init(sprite, resource, 1);
    sprite->mode = 2;
    {
        register u32 *position asm("r5") = p->position;
        {
            u32 zero = 0;
            {
                register u32 *src asm("r0") = &zero;
                u32 *dst = position;
                asm("" : : "r"(src), "r"(dst));
                CpuSet(src, dst, 0x05000002);
            }
        }
        Shadow_Initialize(p->transform, position, 0);
    }
    return ActorShadow_Disable(p->transform);
}
