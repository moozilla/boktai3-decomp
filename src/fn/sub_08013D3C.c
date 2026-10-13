#include "global.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Entry {
    u16 id;
    u8 mode, pad;
    struct Vec position;
    u8 shadow[72];
};
void *sub_08013C14(void);
void *sub_08013C2C(void *, int);
void *sub_08219C40(int);
void sub_08219DD8(void *, int);
int Shadow_Initialize(void *, void *, int);
int sub_08013C4C(void *, void *);
void sub_08219D38(void *);
int ShadowRegistry_Create(int id, int mode, struct Vec *position)
{
    void *m = sub_08013C14();
    struct Entry *p;
    struct Vec *dest;
    if (!m)
        return -1;
    if (sub_08013C2C(m, id))
        return -1;
    p = sub_08219C40(84);
    if (!p)
        return -1;
    sub_08219DD8(p, 84);
    p->id = id;
    dest = &p->position;
    p->position = *position;
    if (Shadow_Initialize(p->shadow, dest, mode == 2) < 0)
        goto fail;
    if (sub_08013C4C(m, p) < 0)
        goto fail;
    p->mode = mode;
    return 0;
fail:
    sub_08219D38(p);
    return -1;
}
