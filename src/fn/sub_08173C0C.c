
#ifdef NONMATCHING
// fuzzy: 86.2%
// foundry draft (worker best 86.2%); what differs is mostly register allocation and instruction order
#define SceneHud_TickEquipmentToggle sub_08173C0C
#define SceneInventory_IsSlotEquipped Inventory_IsEquipmentAssigned
#include "global.h"
#include "scene_hud_display.h"
int SceneInput_NavigateGridKeys(void *,int);
void SceneHud_GetActionIconPosition(void *,int);
void SceneHud_ShowEquipmentDetails(void *);
void Hud_SaveGridSelection(void *);
void MenuDisplay_UpdateActionPrompt(void *,int);
void sub_0816701C(void *,int,int,int,int,int);
s32 Inventory_GetEquipmentId(u32);
u32 Inventory_IsEquipmentAssigned(s32);
s32 sub_0822DD80(u32);
s32 sub_0822DCF4(u32);
u32 InventoryPanel_UpdateSelectedEquipment(void *,u32,u32,u32,u32);
void SceneHud_FillDigitCells(u32,u32,u32,u32);
void InventoryPanel_SetEntryRow(void *,u32,u32,u32,u32,u32);
void SceneHud_RefreshEquipmentIcons(void *,int);
void SceneHud_BeginEquipmentSelection(void *);
void sub_08173C0C(struct SceneHudState *p)
{
    s32 equipped;
    s32 slot;
    void *q;
    q=p->actionInput;
    if(SceneInput_NavigateGridKeys(q,0xf0)) {
        SceneHud_GetActionIconPosition(&p->categoryWidgets[5].position,p->actionCursor);
        SceneHud_ShowEquipmentDetails(p);
        Hud_SaveGridSelection(q);
        MenuDisplay_UpdateActionPrompt(p,2);
        sub_0816701C(p,0,1,0,0,0);
    } else if(gUnk_03005260[1]&1) {
        slot=p->actionCursor;
        if(Inventory_GetEquipmentId(slot)==0xff) Sound_PlaySong(0x192);
        else {
            equipped=Inventory_IsEquipmentAssigned(slot);
            if(equipped) {
                if(sub_0822DD80(slot)) goto success;
                Sound_PlaySong(0x192);
            } else if(sub_0822DCF4(slot)==1) {
                u32 frame=InventoryPanel_UpdateSelectedEquipment(p,0,0,1,equipped);
                SceneHud_FillDigitCells(3,8,3,0);
                InventoryPanel_SetEntryRow(p,8,4,frame,equipped,0x47);
            success:
                Sound_PlaySong(0xdd);
            } else Sound_PlaySong(0x192);
            MenuDisplay_UpdateActionPrompt(p,2);
            SceneHud_RefreshEquipmentIcons(p,0);
        }
    } else if(gUnk_03005260[1]&2) {
        SceneHud_BeginEquipmentSelection(p);
        Sound_PlaySong(0xde);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08173C0C);
#endif
