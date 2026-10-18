#define PlayerStatEffect_Initialize sub_081602D0
#define PlayerStatEffect_SetPhase PlayerBurst_SetStateCallback
#define gEngineFlags gUnk_030053F4
#include "global.h"
#include "player.h"
struct PlayerStatEffect {
    u8 task[0x18];
    struct Player *player;
    u8 unknown1C[0xD8];
    u16 step;
    u16 targetStat;
};
void sub_0815FFDC(struct PlayerStatEffect *);
void sub_08160060(struct PlayerStatEffect *);
void sub_08160108(struct PlayerStatEffect *);
void sub_0815FAF4(struct PlayerStatEffect *);
void PlayerBurst_SetStateCallback(struct PlayerStatEffect *, void (*)(struct PlayerStatEffect *));
extern u32 gUnk_030053F4;
s32 Div(s32, s32);
s32 sub_081602D0(struct PlayerStatEffect *p, struct Player *player)
{
    p->player = player;
    sub_0815FFDC(p);
    sub_08160060(p);
    sub_08160108(p);
    PlayerBurst_SetStateCallback(p, sub_0815FAF4);
    gUnk_030053F4 |= 1;
    p->targetStat = p->player->currentStatA >> 1;
    if (p->targetStat == 0)
        p->targetStat = 1;
    p->step = Div(p->player->currentStatA - p->targetStat, 72) + 1;
    return 0;
}
