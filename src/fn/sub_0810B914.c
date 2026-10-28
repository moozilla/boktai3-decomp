#include "global.h"
#include "engine.h"
#include "ram.h"
#include "area_effect_link.h"
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
/* One u16 slot written through a 16-bit bitfield (an all-ones store
   compiles to a read-or-write). */
struct Bits16 {
    u16 v : 16;
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
void Task_LinkToPriorityList(void *);
void sub_08223180(void);
void Registry1745_ResetTransferCounters(void *);
void Registry1745_InitSessionSettings(void *);
void Registry1745_SetState(void *, s32, void (*)(void));
u8 *Script_GetTextSourcePc(void);
void Hud_SetMode(s32);
void SessionMenu_Destroy(void *);
void ScriptPanel_ConfirmSelection(void);
void Registry1745_RunSession(void);
/* Registry routine 1745: creates the singleton at gLinkSession (0x268
   bytes) from the script options; returns the existing one if there is one,
   or 0 when a required option is missing. */
struct Registry1745 *Registry1745_Create(void)
{
    struct Registry1745 *existing = gLinkSession;
    void *m;
    struct Registry1745 *p;
    struct Registry1745Settings *settings;
    s32 i;
    u16 *entry;
    if (existing)
        return existing;
    {
        void *t = Heap_Alloc(0x268);
        if (!t)
            return 0;
        m = t;
    }
    Memory_Clear(m, 0x268);
    p = m;
    settings = &p->settings;
    gLinkSession = p;
    Task_SetCallbacks(p, 0, SessionMenu_Destroy);
    {
        s32 one = 1;
        u32 zero;
        p->field14 = one;
        p->field16 = one;
        zero = 0;
        p->field10 = (u32)existing;
        Task_LinkToPriorityList(p);
        gClockSynced = one;
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
    for (i = 0; i <= 3; i++) {
        ((struct Bits16 *)entry)->v = 0xFFFF;
        p->field64[i] = 0;
        entry++;
    }
    Registry1745_ResetTransferCounters(m);
    Registry1745_InitSessionSettings(m);
    Registry1745_SetState(p, 1, ScriptPanel_ConfirmSelection);
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
    if (!Script_SeekToKeyword('s') || !(p->fieldD8 = Script_GetTextSourcePc()))
        goto fail;
    if (!Script_SeekToKeyword('L') || !(settings->field174 = Script_GetTextSourcePc()))
        goto fail;
    if (!Script_SeekToKeyword('c') || !(settings->field178 = Script_GetPc())) {
    fail:
        return 0;
    }
    if (!Script_SeekToKeyword('e') || !(settings->field17C = Script_GetTextSourcePc()))
        goto fail;
    Hud_SetMode(7);
    p->fieldB4 = Registry1745_RunSession;
    p->field53 = 1;
}
