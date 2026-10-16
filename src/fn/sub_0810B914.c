#include "global.h"
#include "engine.h"
#ifdef NONMATCHING
// fuzzy: 91.1%
// Open: register choice for the global slot and the allocation (r5/r8), and the
// clear loop, which rematerializes -1 instead of reusing the -1 just stored.
struct Registry1745Settings {
    u8 pad0[0x15C];
    u32 field15C;
    u32 field160;
    u32 field164;
    u8 pad168[0xC];
    u8 *field174;
    u8 *field178;
    u8 *field17C;
};
struct Registry1745 {
    u8 pad0[0x10];
    u16 field10;
    u8 pad12[2];
    u8 field14;
    u8 pad15;
    u8 field16;
    u8 pad17;
    u16 field18;
    u8 pad1A[4];
    u8 field1E;
    u8 pad1F;
    u8 field20;
    u8 pad21[0x1F];
    u16 *field40;
    s32 field44, field48;
    u8 field4C, field4D, field4E, field4F;
    u8 pad50[3];
    u8 field53;
    u8 pad54[0x10];
    u8 field64[4];
    u16 field68[4];
    u8 pad70[0x44];
    void (*fieldB4)(void);
    s32 fieldB8;
    u32 fieldBC, fieldC0, fieldC4, fieldC8, fieldCC, fieldD0, fieldD4;
    u8 *fieldD8;
    u8 padDC[4];
    struct Registry1745Settings settings;
};
extern struct Registry1745 *gUnk_020004B4;
extern u32 gUnk_0300546C;
void sub_08219F74(void *);
void sub_08223180(void);
void sub_08109BF8(void *);
void sub_08109BD0(void *);
void sub_08138154(void *, s32, void (*)(void));
u8 *sub_08227E90(void);
void Hud_SetMode(s32);
void sub_0810B8A8(void *);
void sub_08109D94(void);
void sub_0810B724(void);
/* Registry routine 1745: creates the singleton at gUnk_020004B4 (0x268
   bytes) from the script options; returns the existing one if there is one,
   or 0 when a required option is missing. */
struct Registry1745 *Registry1745_Create(void)
{
    struct Registry1745 *existing = gUnk_020004B4;
    void *m;
    struct Registry1745 *p;
    struct Registry1745Settings *settings;
    s32 i;
    u16 *entry;
    if (existing)
        return existing;
    m = Heap_Alloc(0x268);
    if (!m)
        return 0;
    Memory_Clear(m, 0x268);
    p = m;
    settings = &p->settings;
    gUnk_020004B4 = p;
    Task_SetCallbacks(p, 0, sub_0810B8A8);
    {
        s32 one = 1;
        u32 zero;
        p->field14 = one;
        p->field16 = one;
        zero = 0;
        p->field10 = (u32)existing;
        sub_08219F74(p);
        gUnk_0300546C = one;
        sub_08223180();
        p->field18 = 0xFFFF;
        p->field1E = zero;
        p->field40 = entry = p->field68;
        p->field44 = -1;
        p->field48 = -1;
        p->field4C = zero;
        p->field4D = one;
        p->field4E = zero;
        p->field4F = zero;
        p->fieldB8 = -1;
    }
    {
        s32 mask = -1;
        for (i = 0; i <= 3; i++) {
            *entry |= mask;
            p->field64[i] = 0;
            entry++;
        }
    }
    sub_08109BF8(m);
    sub_08109BD0(m);
    sub_08138154(p, 1, sub_08109D94);
    p->field20 = 3;
    p->fieldD4 = Script_GetKeywordValue('U', 0);
    p->fieldD0 = Script_GetKeywordValue('Y', 0);
    p->fieldBC = Script_GetKeywordValue('l', 0);
    p->fieldC8 = Script_GetKeywordValue('r', 0);
    p->fieldC0 = Script_GetKeywordValue('R', 0);
    settings->field160 = Script_GetKeywordValue('C', 0);
    settings->field15C = Script_GetKeywordValue('V', 0);
    settings->field164 = Script_GetKeywordValue('T', 0);
    p->fieldC4 = Script_GetKeywordValue('v', 0);
    p->fieldCC = Script_GetKeywordValue('E', 0);
    if (!Script_SeekToKeyword('s') || !(p->fieldD8 = sub_08227E90()))
        goto fail;
    if (!Script_SeekToKeyword('L') || !(settings->field174 = sub_08227E90()))
        goto fail;
    if (!Script_SeekToKeyword('c') || !(settings->field178 = Script_GetPc())) {
    fail:
        return 0;
    }
    if (!Script_SeekToKeyword('e') || !(settings->field17C = sub_08227E90()))
        goto fail;
    Hud_SetMode(7);
    p->fieldB4 = sub_0810B724;
    p->field53 = 1;
}
#else
INCLUDE_ASM("asm/nonmatching", Registry1745_Create);
#endif
