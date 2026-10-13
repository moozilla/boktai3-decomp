#include "global.h"
struct Blob {
    u32 words[8];
};
struct LinkMenu {
    u8 task[0x18];
    u8 state, entered, pad1a[2];
    u32 timer;
    int window;
    void *background;
    void *script;
    void *resource;
    struct Blob blob;
    u8 draw50[8];
    u32 flags;
    u8 pad5c[0x54];
};
extern struct LinkMenu *gUnk_020000F8;
extern u16 gUnk_03004BD8, gUnk_03005260[];
extern u32 gUnk_0300523C;
extern u16 gUnk_03004FC0[];
void sub_08033C1C(void);
void PaletteLevels_Reset(void);
void sub_08215490(int);
void sub_08218B30(void);
void sub_08215114(void);
void sub_0821969C(void);
void sub_0821447C(int, void *, void *, void *);
void sub_08177DEC(int);
void sub_082156B8(int);
void *sub_0821A520(int, int);
void sub_082161B4(int, int, void *, int, int, int, void *);
void sub_0821656C(int, int, int, int, int);
void sub_08036C0C(void);
void sub_0822B058(int);
void sub_08036E14(void);
void sub_08036EC8(struct LinkMenu *, int);
int sub_08033690(int, int, int, int);
void sub_0803386C(int, void *);
void sub_08033924(int, int);
void sub_080337FC(int);
void sub_08033778(int);
void sub_0822B2F8(int);
int sub_08036BEC(void);
int sub_0824923C(void *, void *);
extern void *gUnk_08605770[];
void sub_082195E0(void *);
int sub_080370D0(struct LinkMenu *);
int sub_08037110(struct LinkMenu *);
struct LinkMenu *sub_08036E08(void);
struct LinkMenu *sub_08219FBC(int, int);
void sub_0821A04C(void *, void *, void *);
int sub_08037140(struct LinkMenu *);
void sub_0821A0C0(void *);
struct LinkMenu *sub_080371E0(void)
{
    struct LinkMenu *p = sub_08036E08();
    struct LinkMenu *result;
    if (p) {
        result = p;
        goto done;
    }
    {
        struct LinkMenu *allocated = sub_08219FBC(12, 0xB0);
        void *update;
        void *destroy;
        p = allocated;
        if (!p)
            goto returnPointer;
        update = sub_080370D0;
        destroy = sub_08037110;
        sub_0821A04C(allocated, update, destroy);
    }
    if (sub_08037140(p) >= 0)
        goto returnPointer;
    sub_0821A0C0(p);
    result = 0;
    goto done;
returnPointer:
    result = p;
done:
    return result;
}
