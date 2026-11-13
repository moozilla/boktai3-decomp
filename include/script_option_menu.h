#ifndef GUARD_SCRIPT_OPTION_MENU_H
#define GUARD_SCRIPT_OPTION_MENU_H
#include "engine.h"
#include "input.h"
#include "menu_parameters.h"
#include "script_utility.h"
struct OptionMenu {
    struct Task task;
    s32 selection, timer;
};
s32 sub_08249350(s32, s32);
void OptionMenu_Update(struct OptionMenu *p);
void OptionMenu_OnDestroy(struct OptionMenu *menu);
s32 OptionMenu_Initialize(struct OptionMenu *menu);
void OptionMenu_SetScriptsFromScript(void);
struct OptionMenu *OptionMenu_Create(void);
#endif
