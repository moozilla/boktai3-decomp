#include "global.h"
void SelectionMenu_ActorAvailability(u8 *);
void SelectionMenu_PositionInventoryMarkers(u8 *);
void SelectionMenu_PopulateActors(u8 *);
s32 Script_ParseStringRef(s32);
s32 Text_LookupString(void);
void Presentation_UpdateActorGraphic(u8 *, s32);
void sub_0801AC50(u8 *p)
{
    SelectionMenu_ActorAvailability(p);
    SelectionMenu_PositionInventoryMarkers(p);
    SelectionMenu_PopulateActors(p);
    Script_ParseStringRef(*(s32 *)(p + 0xDB0));
    Presentation_UpdateActorGraphic(p, Text_LookupString());
}
