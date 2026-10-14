#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Part {
    u8 pad[6];
    u16 frames;
};
struct Object {
    u8 pad0[0x1c];
    u16 frames, savedFrames;
    u8 pad20[0x3c];
    u8 motion[0xc];
    u8 pos[8];
    u8 pad70[0x64];
    u32 drawable;
    u8 padd8[8];
    struct Part part;
    u8 pade8[0x14];
    u8 transform[4];
    u8 pad100[0x3a];
    u16 modelFrames;
};
int DisplayAnimation_InitializeKeyframes(void *, int, int, void *);
int DisplayAnimation_InitializeSequence(void *, int, int, int, void *);
void sub_08225B30(void *);
void sub_08225B28(void *, void *);
int DisplayAnimation_SetPriority(void *, int);
struct Model {
    u8 pad[0x3a];
    u16 frames;
};
int sub_0802D760(struct Object *p, int kind, int a, int b, int c)
{
    void *d = &p->drawable;
    int status = -1;
    int result;
    if (kind == 2) {
        status = DisplayAnimation_InitializeKeyframes(d, a, c, p->pos);
        {
            register struct Model *m asm("r1") = (struct Model *)p->pad100;
            p->frames = m->frames;
        }
        sub_08225B30(p->motion);
    } else if (kind == 1) {
        status = DisplayAnimation_InitializeSequence(d, a, b, c, p->pos);
        {
            struct Part *part = &p->part;
            p->frames = part->frames;
        }
        sub_08225B28(p->motion, p->transform);
    }
    p->savedFrames = p->frames;
    result = DisplayAnimation_SetPriority(d, 2);
    if (status < 0)
        return -1;
    return result;
}
