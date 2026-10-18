#include "global.h"
struct ModifierSecondaryAction {
    void (*callback)(void);
    u16 config[4];
    s32 kind;
    u8 enabled;
    u8 unknown11[3];
    u32 argument;
};
struct ModifierSecondaryActor {
    u8 unknown0[0x57];
    u8 selection;
    u8 unknown58[0x2d8];
    struct ModifierSecondaryAction action;
};
void Memory_Clear(void *, u32);
void ScriptRecord_FindModifier(u16 *, s32);
void Modifier_EvaluateThirdSourceModeOne(void);
void Modifier_EvaluateThirdSourceModeTwo(void);
void ModifierAction_InitializeSecondary(struct ModifierSecondaryActor *actor, u32 argument)
{
    u8 *selection = (u8 *)actor + 0x48;
    struct ModifierSecondaryAction *action = &actor->action;
    u16 *config;
    action->callback = NULL;
    action->enabled = 0;
    action->argument = argument;
    config = action->config;
    Memory_Clear(config, 12);
    ScriptRecord_FindModifier(config, selection[15]);
    switch (action->kind) {
    case 1:
        action->callback = Modifier_EvaluateThirdSourceModeOne;
        action->enabled = 1;
        break;
    case 2:
        action->callback = Modifier_EvaluateThirdSourceModeTwo;
        action->enabled = 1;
        break;
    }
}
