// MATCHING_CONCESSION: register-pinning
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
    u8 pad4c[0x58];
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
};
void Memory_Clear(void *, int);
#include "rfu_compat.h"
#include "ram.h"
#include "engine_globals.h"
static inline void setOperation(struct Engine *p, int op, int zero)
{
    p->flag = zero;
    p->operation = op;
    p->ticks = zero;
    p->pending = 1;
}
void Solar_PauseMeasurement(void);
void *Heap_Alloc(int);
void Memory_Clear(void *, int);
int SunClock_GetSecond(void);
int Wireless_InitControllerFromScript(struct Engine *);
void Task_SetCallbacks(void *, int, int);
void Task_LinkToPriorityList(void *);
void Heap_Free(void *);
struct Engine *Wireless_CreateController(void)
{
    struct Engine **singleton = &gWirelessController;
    struct Engine *existing = *singleton;
    struct Engine *p;
    int size;
    int random;
    if (existing)
        return existing;
    Solar_PauseMeasurement();
    size = 0x38C;
    p = Heap_Alloc(size);
    if (!p)
        goto failed;
    asm("" : "+r"(p));
    Memory_Clear(p, size);
    random = SunClock_GetSecond();
    {
        u32 *position = &gRandomIndex;
        u32 index = *position + 1;
        u32 mask = 1023;
        const u16 *table;
        int term;
        const u8 *saltAddress;
        int byteMask;
        int salt;
        u16 *destination;
        index &= mask;
        *position = index;
        index <<= 1;
        table = gRandomTable;
        term = *(const u16 *)(index + (u32)table);
        saltAddress = &gSceneEffectPalettePhase;
        byteMask = 255;
        salt = *saltAddress;
        random += term;
        random &= byteMask;
        random <<= 8;
        salt |= random;
        destination = (u16 *)((u8 *)p + 0x6C);
        *destination = salt;
    }
    if (Wireless_InitControllerFromScript(p) >= 0) {
        int one;
        Task_SetCallbacks(p, 0, 0);
        one = 1;
        ((u8 *)p)[0x14] = one;
        ((u8 *)p)[0x16] = one;
        *(u16 *)((u8 *)p + 0x10) = (u32)existing;
        Task_LinkToPriorityList(p);
        gClockSynced = one;
        *singleton = p;
        return p;
    }
    Heap_Free(p);
failed:
    return 0;
}
