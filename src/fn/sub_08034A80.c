#include "global.h"
struct Engine {
    u8 pad0[0x18];
    u8 pad18, bits, pad1a, entered, flag, pad1d, state, flag1f, mode, index;
    u16 mask;
    s8 scroll;
    u8 selected, allowed, count, ids[8], active, flag31, pending, flag33, phase, flag35, flag36,
        pad37;
    u16 operation, value;
    u8 flag3c, flag3d, flag3e, flag3f;
    u16 timer;
    u8 flag42, flag43;
    u32 ticks;
    void *pointer;
    u8 pad4c[4];
    u8 gameName[11], gameIdLow, gameIdHigh, pad5d[3], userName[8], pad68[4];
    u16 gameId;
    u8 pad6e[0x36];
    u32 number;
    u8 work[16];
    u8 padb8[0x34];
    u16 flags, backup, history[5];
    u8 changed, padfb[5];
    u16 diff[5];
    u8 pad10a[2];
    u32 key;
    u16 input[5];
    u8 pad11a[0x1A];
    u8 scratch[64];
    u8 message[0x204];
    void *head, *tail;
    u8 pad380[8];
    void *script;
};
extern struct Engine *gUnk_02000484;
extern u8 gUnk_020000EC;
void Memory_Clear(void *, int);
#define gRfuLinkStatus   gUnk_03006A80
#define gRfuSlotStatusNI gUnk_03006A70
#include "rfu_compat.h"
#include "game_state.h"
static inline void setOperation(struct Engine *p, int op, int zero)
{
    p->flag = zero;
    p->operation = op;
    p->ticks = zero;
    p->pending = 1;
}
// MATCHING_CONCESSION: register-pinning
int Wireless_CheckAdapterReady(void);
int Script_ParseStringRef(const void *);
const u8 *Text_LookupString(int);
int SunClock_GetPhase(void);
int SunClock_GetBaseHour(void);
void WirelessText_EncodeString(const void *, void *, int);
void Wireless_InitSessionConfig(void *, int, int, int, int, const void *, const void *);
extern u16 gUnk_0203B400[];
extern u32 gRandomIndex;
struct Game {
    u8 pad[0x780];
    u8 playerName[8];
};
int WirelessController_UpdateAdapterSetup(struct Engine *p)
{
    u8 *pending = &p->pending;
    u8 *timer;
    register int read asm("r0");
    int zero;
    if (*pending)
        *pending = 0;
    timer = &p->flag3c;
    read = *timer;
    zero = read;
    if (zero) {
        read--;
        *timer = read;
        goto done;
    }
    switch (Wireless_CheckAdapterReady()) {
        case -1:
            setOperation(p, 27, zero);
            break;
        case 0:
            *timer = 60;
            break;
        case 1: {
            int three = 3;
            int base;
            u32 position;
            u16 *table;
            register u32 mask asm("r4");
            u32 *randomIndex;
            int offset;
            int chance;
            const u8 *name;
            u8 *userName;
            u8 *gameName;
            p->flag = zero;
            p->operation = three;
            p->ticks = zero;
            *pending = 1;
            base = Script_ParseStringRef(p->script);
            table = gUnk_0203B400;
            randomIndex = &gRandomIndex;
            position = *randomIndex + 1;
            mask = 1023;
            position &= mask;
            *randomIndex = position;
            chance = ((u8 *)table)[position * 2];
            if (chance <= 63) {
                u32 form = SunClock_GetPhase();
                int stage = SunClock_GetBaseHour();
                if (form - 1 <= 2) {
                    offset = 1;
                    if (stage <= 11)
                        offset = 0;
                } else {
                    offset = 3;
                    if (stage <= 11)
                        offset = 2;
                }
            } else {
                u32 next = (position + 1) & mask;
                *randomIndex = next;
                offset = (table[next] & three) + 4;
            }
            name = Text_LookupString(base + offset);
            WirelessText_EncodeString(gGameState->playerName, p->userName, 8);
            userName = p->userName;
            gameName = p->gameName;
            WirelessText_EncodeString(name, gameName, 11);
            p->gameIdLow = p->gameId;
            p->gameIdHigh = p->gameId >> 8;
            {
                void *config = (u8 *)p + 0x11C;
                int mode = p->active;
                int flags = 0;
                if (mode)
                    flags = 3;
                Wireless_InitSessionConfig(config, mode, 43, flags, 0, gameName, userName);
            }
            break;
        }
        default:
            break;
    }
done:
    return 0;
}
