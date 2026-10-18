#define SceneHud_BeginEquipmentSelection sub_08173A7C
#define SceneHud_RefreshEquipmentIcons sub_0817373C
#define SceneHud_ShowWidget PauseMenu_ClearSpriteFlags
#define SceneHud_HideWidget PauseMenu_AddSpriteFlags
#include "global.h"
#include "scene_hud_display.h"
void sub_0817373C(void *,int);
void Hud_RestoreSecondaryGridSelection(void *);
void sub_08173508(void *,int);
void PauseMenu_ClearSpriteFlags(void *,int,int);
void PauseMenu_AddSpriteFlags(void *,int,int);
void sub_081672FC(void *);
void sub_08166B7C(void *,int);
void sub_08166534(void *,int,int);
void PauseMenu_RefreshEquipmentPanel(void *);
void Hud_UpdateInventoryMenuInput(void);
void Hud_SetInteractionCallback(void *,void (*)(void),int);
void sub_081663A0(void *,int,int,int,int);
void sub_08166ED4(void *,int,int,int,int);
void sub_08166F84(void *,int,int,int,int);
void sub_08173450(void *);
void sub_08173A7C(struct SceneHudState *p)
{
    void *q;
    int z;
    sub_0817373C(p,0);
    q=p->actionInput;
    Hud_RestoreSecondaryGridSelection(q);
    sub_08173508(q,0xe1);
    PauseMenu_ClearSpriteFlags(p,4,1);
    PauseMenu_AddSpriteFlags(p,6,1);
    PauseMenu_AddSpriteFlags(p,5,1);
    sub_081672FC(p);
    sub_08166B7C(p,2);
    sub_08166534(p,10,6);
    PauseMenu_RefreshEquipmentPanel(p);
    Hud_SetInteractionCallback(p,Hud_UpdateInventoryMenuInput,0);
    z=0;
    sub_081663A0(p,0,1,0,z);
    sub_08166ED4(p,8,3,0,z);
    sub_08166F84(p,8,4,0,z);
    sub_08173450(p);
}
