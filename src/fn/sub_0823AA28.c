#define Player_InitializeActionState sub_0823AA28
#define gPlayerActionCallbacks       gUnk_08E879D8
#include "global.h"
#include "player.h"
u32 LinkPlayer_GetInitialActionCount(struct Player *, const struct PlayerActionConfig *);
void sub_0823A5EC(struct Player *, const struct PlayerActionConfig *);
void sub_0823A830(struct Player *);
void sub_0823F56C(void);
extern void (*const gUnk_08E879D8[])(void);
void sub_0823AA28(struct Player *s, const struct PlayerActionConfig *config)
{
    s->statModifierContext.actionKind = config->kind;
    s->statModifierContext.actionDuration = config->duration;
    s->statModifierContext.actionPhase = LinkPlayer_GetInitialActionCount(s, config);
    s->statModifierContext.actionTimer = 0;
    sub_0823A5EC(s, config);
    if (s->statModifierContext.actionKind == 0x30)
        s->actionCallback = sub_0823F56C;
    else
        s->actionCallback = gUnk_08E879D8[s->statModifierContext.statBOverrideEnabled];
    sub_0823A830(s);
}
