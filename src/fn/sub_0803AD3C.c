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
void sub_0801683C(void);
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
void CpuSet(const void *, void *, u32);
void sub_08036C0C(void);
void sub_0822B058(int);
void sub_08036E14(struct LinkMenu *);
void sub_0803AD3C(struct LinkMenu *, int);
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
void sub_0803AD3C(struct LinkMenu *p, int layer)
{
    void *resource = sub_0821A520(0xC091, 0xCF15);
    int zero;
    p->resource = resource;
    zero = 0;
    sub_082161B4(1, 0, resource, 0, zero, 1, &layer);
    sub_0821656C(2, 0, 0, 0, zero);
    CpuSet((u8 *)sub_0821A520(0x92B3, 0x26BB) + 0x14, gUnk_03004FC0, 256);
}