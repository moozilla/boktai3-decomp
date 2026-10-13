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
void sub_082151E4(void *, int);
void sub_08215284(void *, int);
void sub_082144A4(void *, void *, int);
void sub_080135E8(void *, void *, int);
int sub_08013440(void *);
#define BoardPiece_Initialize sub_0800F9C4
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
    sub_082151E4(resource, 0xDA6D);
    {
        void *r = resource;
        asm("" : "+r"(r));
        sub_08215284(r, 0x2D3);
    }
    resource->palette = owner + 0xCA8;
    sprite = (void *)p->sprite;
    sub_082144A4(sprite, resource, 1);
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
        sub_080135E8(p->transform, position, 0);
    }
    return sub_08013440(p->transform);
}
