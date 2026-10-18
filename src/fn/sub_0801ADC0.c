#include "global.h"
// MATCHING_CONCESSION: register-pinning
extern u16 gUnk_03005260[];
int SelectionMenu_ConsumeCallbackFlag(void *);
void SelectionMenu_RefreshActorPage(void *);
void SelectionMenu_RefreshItemPage(void *);
int sub_0801ACB8(void *);
void Sound_PlaySong(int);
void Presentation_UpdateActorGraphic(void *, int);
void Presentation_UpdateItemGraphic(void *, int);
u8 *Inventory_GetEquipmentEntry(int);
int Inventory_GetConsumableId(int);
void SelectionMenu_StartAnimateChoice(void *);
void SelectionMenu_Back(void *);
void SelectionMenu_Choose(u8 *p)
{
    int index;
    switch (p[0x1e]) {

        case 0:
            if (SelectionMenu_ConsumeCallbackFlag(p))
                SelectionMenu_RefreshActorPage(p);
            if (gUnk_03005260[0] & 4) {
                if (gUnk_03005260[1] & 1) {
                    if (++p[0x24] > 2)
                        p[0x24] = 0;
                    Sound_PlaySong(0xdc);
                }
            } else {
                index = sub_0801ACB8(p);
                if (index != p[0x1d]) {
                    {
                        register int x asm("r0") = (index & 3) * 24 + 144;
                        register int offset asm("r3");
                        offset = 0x1b4;
                        asm("" : "+r"(offset));
                        *(u16 *)(p + offset) = x;
                        x = (index >> 2) * 24 + 32;
                        offset += 2;
                        {
                            register u16 *out asm("r1") = (u16 *)(p + offset);
                            *out = x;
                        }
                    }
                    p[0x1d] = index;
                    Sound_PlaySong(0xdc);
                    Presentation_UpdateActorGraphic(p, 0);
                }
                if (gUnk_03005260[1] & 1) {
                    if (*Inventory_GetEquipmentEntry(p[0x1d]) != 255 &&
                        ((1 << p[0x1d]) & *(u32 *)(p + 0x34)))
                        SelectionMenu_StartAnimateChoice(p);
                    else
                        Sound_PlaySong(0x192);
                    return;
                } else if (gUnk_03005260[1] & 2) {
                    SelectionMenu_Back(p);
                    return;
                }
            }
            break;

        case 1:
            if (SelectionMenu_ConsumeCallbackFlag(p))
                SelectionMenu_RefreshItemPage(p);
            if (gUnk_03005260[0] & 4) {
                if (gUnk_03005260[1] & 1) {
                    if (++p[0x24] > 2)
                        p[0x24] = 0;
                    Sound_PlaySong(0xdc);
                }
            } else {
                index = sub_0801ACB8(p);
                {
                    register int old asm("r3") = p[0x1d];
                    asm("" : "+r"(old));
                    if (index != old) {
                        {
                            register int x asm("r0") = (index & 3) * 24 + 144;
                            register int offset asm("r3");
                            offset = 0x1b4;
                            asm("" : "+r"(offset));
                            *(u16 *)(p + offset) = x;
                            x = (index >> 2) * 24 + 32;
                            offset += 2;
                            {
                                register u16 *out asm("r1") = (u16 *)(p + offset);
                                *out = x;
                            }
                        }
                        p[0x1d] = index;
                        Sound_PlaySong(0xdc);
                        Presentation_UpdateItemGraphic(p, 0);
                    }
                }
                if (gUnk_03005260[1] & 1) {
                    if (Inventory_GetConsumableId(p[0x1d]) >= 0) {
                        int selected = p[0x1d];
                        if ((1 << selected) & *(u32 *)(p + 0x34))
                            SelectionMenu_StartAnimateChoice(p);
                        else
                            Sound_PlaySong(0x192);
                    } else
                        Sound_PlaySong(0x192);
                    return;
                } else if (gUnk_03005260[1] & 2) {
                    SelectionMenu_Back(p);
                    return;
                }
            }
            break;
    }
    (*(u32 *)(p + 0x28))++;
}
