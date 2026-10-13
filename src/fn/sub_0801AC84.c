#include "global.h"
void SelectionMenu_ItemAvailability(u8 *);
void SelectionMenu_PositionEquipmentMarkers(u8 *);
void SelectionMenu_PopulateItems(u8 *);
s32 Script_ParseStringRef(s32);
s32 Text_LookupString(void);
void Presentation_UpdateItemGraphic(u8 *, s32);
void sub_0801AC84(u8 *p)
{
    SelectionMenu_ItemAvailability(p);
    SelectionMenu_PositionEquipmentMarkers(p);
    SelectionMenu_PopulateItems(p);
    Script_ParseStringRef(*(s32 *)(p + 0xDB0));
    Presentation_UpdateItemGraphic(p, Text_LookupString());
}
