#include "global.h"

struct PanelPlayer {
    u8 pad0[0x30];
    s16 x;
    u16 y;
    u16 z;
    u8 pad36[0x3A2 - 0x36];
    u8 flag3A2;
    u8 flag3A3;
    u8 pad3A4[0x458 - 0x3A4];
    u8 kind;
    u8 pad459;
    u16 timer;
    u8 pad45C[0x522 - 0x45C];
    u16 speed;
};

struct PanelState {
    u8 pad0[0x50C];
    s32 balance;
};

struct ControlPanel {
    u8 pad0[0x57A];
    u16 cooldown;
    u16 f57C;
    u16 f57E;
    u8 pad580[0x588 - 0x580];
    u16 f588;
    u8 pad58A[0x590 - 0x58A];
    u16 multiplier;
    u8 pad592[2];
    s32 ticks;
    s32 tickLimit;
    s32 total;
    s32 totalLimit;
};

extern struct PanelPlayer *volatile gPlayers;
extern struct PanelState *gGameState;
void ControlPanel_UpdateGaugeIndicator(struct ControlPanel *);
void Sound_StopSong(u32);
void Sound_SetChannel10Tempo(u32);
void ControlPanel_ClearPlayerInputHistory(void);
void ControlPanel_UpdateSustainedInputTimer(struct ControlPanel *);
s32 sub_081C1E48(struct ControlPanel *);
void ControlPanel_BeginIdle(struct ControlPanel *, u32);
void ControlPanel_ShowRewardBalanceDigits(struct ControlPanel *);
void Camera_RequestShake(u32);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void SegmentDisplay_UpdatePlayerSegments(struct ControlPanel *);
void ControlPanel_TickMultiplierTimer(struct ControlPanel *);

void sub_081C1FC0(struct ControlPanel *p)
{
    struct PanelPlayer *pl = gPlayers;
    u8 *kindp = &pl->kind;
    u32 kind = *kindp;

    if (kind == 0) {
        s32 done;
        s32 v;
        s32 total;
        struct PanelPlayer *q;

        p->ticks++;
        ControlPanel_UpdateGaugeIndicator(p);
        done = 0;
        q = gPlayers;
        q->x -= q->speed - 3;
        {
            struct PanelPlayer *r = gPlayers;
            s16 x = r->x;
            q = r;
            if (x < 0x380) {
                q->x = 0x380;
            } else if (x > 0x3FF) {
                q->x = 0x3FF;
                done = 1;
            }
        }
        if (done) {
            gPlayers->kind = 1;
            p->f57E++;
            Sound_StopSong(0x23A);
            Sound_StopSong(0x23B);
            Sound_StopSong(0x23C);
            Sound_StopSong(0x23D);
            p->f57C = 0;
            p->multiplier = 1;
            p->f588 = 0;
            Sound_SetChannel10Tempo(0x100);
            ControlPanel_ClearPlayerInputHistory();
        } else {
            ControlPanel_UpdateSustainedInputTimer(p);
            v = sub_081C1E48(p);
            total = p->total += v;
            if (gGameState->balance < v) {
                gGameState->balance = done;
                if (p->cooldown == 0)
                    ControlPanel_BeginIdle(p, 0);
            } else if (p->ticks >= p->tickLimit && total >= p->totalLimit) {
                ControlPanel_BeginIdle(p, 1);
            } else {
                Sound_SetChannel10Tempo(gPlayers->speed * 96);
                gGameState->balance -= v;
            }
        }
        ControlPanel_ShowRewardBalanceDigits(p);
        if (p->multiplier > 1)
            Camera_RequestShake(p->multiplier * 4);
    } else if ((*(u32 *)kindp & 0xFFFF00FF) == 0x10002) {
        ScreenTransition_SendCommand(1, 5, 4, 4, 4, 0x1FFF, 0);
    } else if (kind == 3) {
        if (pl->timer > 0x1F) {
            s32 zero = 0;
            s32 four = 4;
            *kindp = four;
            gPlayers->timer = zero;
            gPlayers->x = 0x380;
            gPlayers->y = 0x100;
            gPlayers->z = 0x580;
            gPlayers->flag3A2 = 1;
            gPlayers->flag3A3 = 1;
            ScreenTransition_SendCommand(0, 5, four, 4, four, 0x1FFF, zero);
        }
    } else if (kind == 4) {
        if (pl->timer > 0x1F) {
            s32 zero = 0;
            *kindp = zero;
            gPlayers->timer = zero;
            p->f588 = zero;
        }
    }
    SegmentDisplay_UpdatePlayerSegments(p);
    ControlPanel_TickMultiplierTimer(p);
    if (p->cooldown)
        p->cooldown--;
}
