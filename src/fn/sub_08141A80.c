#define Player_BeginVariantAction     sub_08141A80
#define Player_SelectHistoryDirection sub_08141A1C
#define ActorVariant_GetTableValue    sub_081419D4
#include "global.h"
#include "player.h"
u32 Player_ConsumeStateEntry(struct Player *);
s32 sub_08141A1C(struct Player *);
void sub_0813ADD0(struct Player *);
s32 sub_081419D4(u32, s32);
s32 sub_0813AE18(struct Player *, s32, s32, s32);
void sub_08141A80(struct Player *p)
{
    if (Player_ConsumeStateEntry(p)) {
        if (p->historyDirectionMode == 1)
            p->facing = sub_08141A1C(p);
        sub_0813ADD0(p);
        p->actionResetCounter = 0;
    }
    p->statusFlags &= ~1;
    sub_0813AE18(p, sub_081419D4(p->actionMode, 0), 0x40, 1);
}
