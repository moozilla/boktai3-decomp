#include "global.h"

struct ConfirmPanel {
    u8 f0[0x1C];
    u32 f1C;
    u8 f20[0x388 - 0x20];
    u32 f388;
    u8 f38C[0x470 - 0x38C];
    u8 snapshot[0xCE4 - 0x470];
    u32 callback;
    u32 fCE8;
    u32 fCEC;
    u32 fCF0;
    u32 fCF4;
    u8 fCF8[0xD04 - 0xCF8];
    u8 fD04;
    u8 fD05[0xD24 - 0xD05];
    u8 fD24;
    u8 fD25;
    u8 fD26;
    u8 fD27;
    u8 fD28;
    u8 fD29;
    u8 fD2A;
    u8 fD2B;
    u8 fD2C;
    u8 fD2D[0xD30 - 0xD2D];
    u32 fD30;
    u32 fD34;
};
extern u32 gEngineFlags;
extern u32 gUnk_030053F0;
extern u32 gUnk_020001CC;
s32 Script_SeekToKeyword(s32);
void *Script_GetTextSourcePc(void);
void *Script_GetPc(void);
s32 Script_GetKeywordValue(s32, s32);
void Bg_ClearTilemap(s32);
void Registry3C9F_LoadBackground(void *);
void SelectionPanel_InitEntryIds(void *);
void sub_0810DB0C(void *);
void SelectionPanel_InitPreview(void *);
void sub_0810DC7C(void *);
void ModifierMenu_InitSelectionSnapshot(void *, u32);
void sub_0810D760(void *);
void RecordPanel_ShowName(void *);
void Registry3C9F_InitTextState(void *);
void SelectionPanel_UpdateDescription(void *);
void sub_0810E4DC(void);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);

s32 sub_0810E6E4(struct ConfirmPanel *p)
{
    s32 zero;
    u32 cb;
    if (Script_SeekToKeyword(0x73)) {
        p->fCE8 = (u32)Script_GetTextSourcePc();
        if (p->fCE8 == 0)
            return -1;
    }
    if (Script_SeekToKeyword(0x63)) {
        p->fCEC = (u32)Script_GetTextSourcePc();
        if (p->fCEC == 0)
            return -1;
    }
    if (Script_SeekToKeyword(0x61)) {
        p->fCF0 = (u32)Script_GetTextSourcePc();
        if (p->fCF0 == 0)
            return -1;
    }
    if (Script_SeekToKeyword(0x6e)) {
        p->fCF4 = (u32)Script_GetPc();
        if (p->fCF4 == 0)
            return -1;
    }
    {
        u32 mask = 0x800;
        u32 m = (gEngineFlags | gUnk_030053F0) & mask;
        if (m) {
            p->fD26 = 1;
            p->fD28 = 0;
            p->fD29 = 5;
            p->fD25 = 0;
            p->fD2A = 0;
            p->fD24 = 0;
        } else {
            p->fD26 = m;
            p->fD28 = 1;
            p->fD29 = 5;
            p->fD25 = m;
            p->fD2A = m;
            p->fD24 = m;
        }
    }
    p->fD34 = Script_GetKeywordValue(0x65, 0);
    p->fD30 = Script_GetKeywordValue(0x43, 0);
    Bg_ClearTilemap(3);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(0);
    Registry3C9F_LoadBackground(p);
    SelectionPanel_InitEntryIds(p);
    sub_0810DB0C(p);
    SelectionPanel_InitPreview(p);
    sub_0810DC7C(p);
    ModifierMenu_InitSelectionSnapshot(p->snapshot, p->f1C);
    sub_0810D760(p);
    RecordPanel_ShowName(p);
    Registry3C9F_InitTextState(p);
    SelectionPanel_UpdateDescription(p);
    cb = (u32)sub_0810E4DC;
    zero = 0;
    p->callback = cb;
    p->fD2C = 1;
    p->fD04 = zero;
    p->f388 |= 1;
    if (Script_GetKeywordValue(0x66, 0)) {
        ScreenTransition_SendCommand(2, 5, 4, 4, 4, 0xFFFF, zero);
    }
    gUnk_020001CC = (u32)p;
    return 1;
}
