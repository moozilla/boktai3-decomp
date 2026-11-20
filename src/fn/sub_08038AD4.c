#include "wireless_trade_menu.h"
#include "state_actor.h"
#include "global.h"
#include "render.h"
#include "engine_globals.h"
#include "player.h"
#include "ram.h"
void Hud_RegisterTimer(u16 *);
void WirelessGameMenu_SetStatusPanelState(struct WirelessGameExchange *, int);
void Sound_PlayTrackedSong(int);
void WirelessGameMenu_UpdateHealthIndicators(struct WirelessGameExchange *);
void WirelessGameMenu_SignalStatusPanel(struct WirelessGameExchange *);
int Mod(int, int);
int Div(int, int);
struct StateActor *SceneActors_GetBySlot(u32);
void WirelessGame_AddWeightedScore(int, int);
int WirelessExchange_SendPacket(struct WirelessGameExchange *, int, const u32 *, int);
void Hud_ClearTimer(void);
void Sound_PlaySong(int);
static inline u32 test(u32 *flags, u32 mask)
{
    return *flags & mask;
}
static inline void set(u32 mask)
{
    gEnginePauseFlags |= mask;
}
void WirelessGameMenu_UpdateActiveRound(struct WirelessGameExchange *p)
{
    u32 data;
    u8 *entered = &p->entered;
    if (*entered) {
        *entered = 0;
        p->roundTimeLeft = WIRELESS_ROUND_FRAMES;
        Hud_RegisterTimer(&p->roundTimeLeft);
        WirelessGameMenu_SetStatusPanelState(p, 4);
        Sound_PlayTrackedSong(22);
        p->roundActive = 1;
    }
    WirelessGameMenu_UpdateHealthIndicators(p);
    if (p->timer == 30) {
        WirelessGameMenu_SignalStatusPanel(p);
    }
    if (Mod(p->timer, 60) == 0) {
        struct Player *actor = gPlayers[0];
        struct StateActor *target = SceneActors_GetBySlot(0);
        int a, b;
        struct WirelessGamePeerStats *e;
        u32 packed;
        int total;
        int i;
        int shift;
        u8 *q;
        if (actor && actor->maximumStatA) {
            a = Div(100 * actor->currentStatA, actor->maximumStatA);
        } else {
            a = 0;
        }
        if ((a == 0) && actor->currentStatA) {
            a = 1;
        }
        if (target && (target->maxHealth > 0)) {
            b = Div(100 * target->health, target->maxHealth);
        } else {
            b = 0;
        }
        do {
            if ((b == 0) && (target->health > 0)) {
                b = 1;
            }
            data = a | (b << 8);
            e = &p->local;
            packed = 0;
            total = 0;
            i = 0;
            shift = 0;
            q = e->counts;
        } while (0);
        do {
            int value;
            u32 flag = e->itemMode;
            if (flag) {
                value = *q;
            } else {
                int old = *q;
                u8 *score;
                int n;
                score = p->queuedCounts;
                score += i;
                n = *score;
                value = old + n;
                total += n;
                *score = (float)flag;
            }
            e->counts[i] = 0;
            if (value > 15) {
                value = 15;
            }
            {
                int count = shift;
                u32 merged;
                value <<= count;
                merged = packed;
                merged |= value;
                packed = merged;
            }
            q++;
            i++;
            shift += 4;
        } while (i <= 3);
        if (total > 0) {
            WirelessGame_AddWeightedScore(3, 1);
        }
        data |= packed << 16;
        WirelessExchange_SendPacket(p, 0, &data, 1);
    }
    if (p->playerDefeated || p->opponentDefeated || test(&p->exchangeFlags, 128)) {
        Hud_ClearTimer();
        WirelessGameMenu_EnterState(p, WIRELESS_MENU_ROUND_RESULT_EXCHANGE);
        return;
    }
    if (!--p->roundTimeLeft) {
        Hud_ClearTimer();
        Sound_PlaySong(0x372);
        WirelessGameMenu_EnterState(p, WIRELESS_MENU_ROUND_HANDSHAKE);
        set(4);
        p->roundActive = 0;
    } else {
        p->timer++;
    }
}
