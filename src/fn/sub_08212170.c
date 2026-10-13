#include "global.h"
s32 sub_082118A0(u32);
s32 sub_082118D8(u32);
void TextWindow_SetScript(void *, void *);
void TextWindow_Refresh(void *, u32);
void sub_08033A38(void *, u32, void *);
void sub_080337FC(void *);
void sub_08033468(void);
void sub_08174578(void *, void *);
void sub_082119FC(void *, void *);
extern u8 gUnk_08E6092C[], gUnk_08E60F80[];
u32 sub_08212170(u8 *p)
{
    s32 v;
    if (*(s16 *)(p + 0xb60) <= 5) {
        s16 *idx = (s16 *)(p + 0xb68);
        s32 idxvalue = *idx;
        u8 *tbl = p + 0xb84;
        v = sub_082118A0(tbl[idxvalue]);
        if (v) {
            u8 *flag;
            void **obj = (void **)(p + 0xb74);
            void *objvalue = *obj;
            void **arg = (void **)(p + 0xb78);
            TextWindow_SetScript(objvalue, *arg);
            flag = p + 0xbec;
            *flag = 0;
            TextWindow_Refresh(*obj, tbl[*idx]);
            sub_08033A38(*obj, 0, flag);
            sub_080337FC(*obj);
            sub_08033468();
            sub_08174578(gUnk_08E6092C + tbl[*idx] * 32, *arg);
        } else {
            u8 *flag;
            void **obj = (void **)(p + 0xb74);
            TextWindow_SetScript(*obj, *(void **)(p + 0xb78));
            flag = p + 0xbec;
            *flag = v;
            TextWindow_Refresh(*obj, 0x62);
            sub_08033A38(*obj, 0, flag);
            sub_080337FC(*obj);
            sub_08033468();
        }
    } else {
        s16 *idx = (s16 *)(p + 0xb68);
        s32 idxvalue = *idx;
        u8 *tbl = p + 0xb84;
        v = sub_082118D8(tbl[idxvalue]);
        if (v) {
            u8 *flag;
            void **obj = (void **)(p + 0xb74);
            void *objvalue = *obj;
            void **arg = (void **)(p + 0xb80);
            TextWindow_SetScript(objvalue, *arg);
            flag = p + 0xbec;
            *flag = 0;
            TextWindow_Refresh(*obj, tbl[*idx]);
            sub_08033A38(*obj, 0, flag);
            sub_080337FC(*obj);
            sub_08033468();
            sub_082119FC(gUnk_08E60F80 + tbl[*idx] * 16, *arg);
        } else {
            u8 *flag;
            void **obj = (void **)(p + 0xb74);
            TextWindow_SetScript(*obj, *(void **)(p + 0xb78));
            flag = p + 0xbec;
            *flag = v;
            TextWindow_Refresh(*obj, 0x62);
            sub_08033A38(*obj, 0, flag);
            sub_080337FC(*obj);
            sub_08033468();
        }
    }
    return 0;
}
