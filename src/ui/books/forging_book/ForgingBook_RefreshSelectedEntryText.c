#include "global.h"
s32 ForgingBook_IsWeaponDiscovered(u32);
s32 ForgingBook_IsItemDiscoveryRecorded(u32);
void TextWindow_SetScript(void *, void *);
void TextWindow_Refresh(void *, u32);
void TextWindow_SetString(void *, u32, void *);
void TextWindow_StartPlayback(void *);
void ScrollingDialogue_ClearScript(void);
void Presentation_ApplyPackedGraphicData(void *, void *);
void ForgingBook_SetItemNameText(void *, void *);
extern u8 gUnk_08E6092C[], gUnk_08E60F80[];
u32 ForgingBook_RefreshSelectedEntryText(u8 *p)
{
    s32 v;
    if (*(s16 *)(p + 0xb60) <= 5) {
        s16 *idx = (s16 *)(p + 0xb68);
        s32 idxvalue = *idx;
        u8 *tbl = p + 0xb84;
        v = ForgingBook_IsWeaponDiscovered(tbl[idxvalue]);
        if (v) {
            u8 *flag;
            void **obj = (void **)(p + 0xb74);
            void *objvalue = *obj;
            void **arg = (void **)(p + 0xb78);
            TextWindow_SetScript(objvalue, *arg);
            flag = p + 0xbec;
            *flag = 0;
            TextWindow_Refresh(*obj, tbl[*idx]);
            TextWindow_SetString(*obj, 0, flag);
            TextWindow_StartPlayback(*obj);
            ScrollingDialogue_ClearScript();
            Presentation_ApplyPackedGraphicData(gUnk_08E6092C + tbl[*idx] * 32, *arg);
        } else {
            u8 *flag;
            void **obj = (void **)(p + 0xb74);
            TextWindow_SetScript(*obj, *(void **)(p + 0xb78));
            flag = p + 0xbec;
            *flag = v;
            TextWindow_Refresh(*obj, 0x62);
            TextWindow_SetString(*obj, 0, flag);
            TextWindow_StartPlayback(*obj);
            ScrollingDialogue_ClearScript();
        }
    } else {
        s16 *idx = (s16 *)(p + 0xb68);
        s32 idxvalue = *idx;
        u8 *tbl = p + 0xb84;
        v = ForgingBook_IsItemDiscoveryRecorded(tbl[idxvalue]);
        if (v) {
            u8 *flag;
            void **obj = (void **)(p + 0xb74);
            void *objvalue = *obj;
            void **arg = (void **)(p + 0xb80);
            TextWindow_SetScript(objvalue, *arg);
            flag = p + 0xbec;
            *flag = 0;
            TextWindow_Refresh(*obj, tbl[*idx]);
            TextWindow_SetString(*obj, 0, flag);
            TextWindow_StartPlayback(*obj);
            ScrollingDialogue_ClearScript();
            ForgingBook_SetItemNameText(gUnk_08E60F80 + tbl[*idx] * 16, *arg);
        } else {
            u8 *flag;
            void **obj = (void **)(p + 0xb74);
            TextWindow_SetScript(*obj, *(void **)(p + 0xb78));
            flag = p + 0xbec;
            *flag = v;
            TextWindow_Refresh(*obj, 0x62);
            TextWindow_SetString(*obj, 0, flag);
            TextWindow_StartPlayback(*obj);
            ScrollingDialogue_ClearScript();
        }
    }
    return 0;
}
