#include "wireless_connection_controller.h"

u32 WirelessController_GetInstance(void)
{
    return (u32)gUnk_02000484;
}

s32 WirelessController_IsCreated(void)
{
    if (gUnk_02000484)
        return 1;
    return 0;
}

extern u8 gUnk_020000EC;

void WirelessController_ResetSession(struct WirelessController *p)
{
    int zero = 0;
    int i;
    u8 *ids;
    u8 *global;
    int byteZero;
    int secondZero, secondByteZero, one, clearZero;
    u8 *active;
    p->advanceRequested = zero;
    p->connectedSlots = zero;
    p->previousConnectedSlots = zero;
    p->connectionState = zero;
    p->participantIndex = zero;
    p->sendSlot = zero;
    p->parentSlot = zero;
    byteZero = 0;
    p->handshakeFlags = zero;
    p->peerScroll = byteZero;
    p->selectedPeer = byteZero;
    p->availableSlots = byteZero;
    p->peerCount = byteZero;
    global = &gUnk_020000EC;
    clearZero = 0;
    i = 7;
    ids = &p->peerIds[7];
    do {
        *ids = clearZero;
        ids--;
        i--;
    } while (i >= 0);
    active = &p->active;
    secondZero = 0;
    one = 1;
    *active = one;
    p->inputReady = secondZero;
    p->sessionMode = secondZero;
    p->lostSlots = secondZero;
    p->sendReady = secondZero;
    p->advanceRequested = secondZero;
    secondByteZero = 0;
    p->operation = secondZero;
    p->operationTicks = secondZero;
    p->operationEntered = one;
    p->operationValue = secondZero;
    p->setupDelay = secondByteZero;
    p->resetRequested = secondByteZero;
    p->inputBlocked = secondByteZero;
    p->disconnectMode = secondByteZero;
    p->operationTimer = secondZero;
    p->unk42 = secondByteZero;
    p->unk43 = secondByteZero;
    p->frameCounter = 0;
    *global = secondByteZero;
    p->resyncRequested = secondByteZero;
    p->sessionMode = secondByteZero;
    p->onSend = 0;
    p->onReceive = 0;
    Memory_Clear(p->peerRoster, 64);
    Memory_Clear(&p->handshakeNumber, 4);
    Memory_Clear(p->handshakeWork, 16);
}

void WirelessController_ResetPeerExchange(struct WirelessController *p)
{
    int zero = 0;
    p->connectionState = zero;
    p->handshakeFlags = zero;
    p->lostSlots = zero;
    p->sendReady = zero;
    p->setupDelay = zero;
    p->resetRequested = zero;
    p->inputBlocked = zero;
    p->disconnectMode = zero;
    Memory_Clear(p->peerRoster, 64);
}

u32 WirelessController_SetLocalInput(struct WirelessController *p, u16 v)
{
    p->savedLocalInput = v;
}

int WirelessController_RestoreLocalInput(struct WirelessController *p)
{
    int result;
    if (!(p->localInput & 0x4000)) {
        result = -1;
        goto done;
    }
    p->localInput = p->savedLocalInput;
    result = 0;
done:
    return result;
}

int sub_08224E54(void *, void *);

int WirelessController_PollInputExchange(struct WirelessController *p)
{
    u8 *changed;
    int result;
    void *key = &p->exchangeKey;
    void *message = p->packetQueue;
    if (sub_08224E54(message, key)) {
        u16 *src;
        u16 *diff;
        u16 *history;
        int i;
        changed = &p->inputChanged;
        src = p->exchangedInput;
        diff = p->inputDiff;
        history = p->inputHistory;
        i = 4;
        do {
            *history = *src;
            *diff ^= *src;
            src++;
            diff++;
            history++;
            i--;
        } while (i >= 0);
        result = 0;
    } else {
        result = -2;
        changed = &p->inputChanged;
    }
    {
        int one = 1;
        *changed = one;
    }
    return result;
}

u32 WirelessController_ConsumePeerRefreshRequest(struct WirelessController *p)
{
    u32 r;
    if (p->peerRefreshRequested == 0) {
        r = 0;
    } else {
        p->peerRefreshRequested = 0;
        r = 1;
    }
    return r;
}

u32 WirelessController_ConsumeAdvanceRequest(struct WirelessController *p)
{
    u32 r;
    if (p->advanceRequested == 0) {
        r = 0;
    } else {
        p->advanceRequested = 0;
        r = 1;
    }
    return r;
}

void WirelessController_RebuildAvailablePeerList(struct WirelessController *p)
{
    int i;
    int left, right, limit;
    p->peerCount = 0;
    i = 0;
    for (; i < gUnk_03006A80->findParentCount; i++) {
        if ((p->availableSlots >> i) & 1) {
            {
                struct RfuTgtData *base = gUnk_03006A80->partner;
                int offset;
                offset = i << 5;
                base = (struct RfuTgtData *)((u32)base + offset);
                if (base->slot <= 3) {
                    p->peerIds[p->peerCount] = i;
                    p->peerCount++;
                }
            }
        }
    }
    left = p->peerScroll - 2;
    right = p->peerScroll + 3;
    if (left < 0) {
        i = left;
        do {
            i++;
        } while (i < 0);
    }
    i = left;
    if (i < 0)
        i = 0;
    goto test;
loop:
    i++;
test:
    limit = right;
    if (right > 8)
        limit = 8;
    if (i < limit)
        goto loop;
    if (right > 7 && i < right) {
        i = right - i;
        do {
            i--;
        } while (i);
    }
}

void sub_08221380(const void *, void *, int);

// MATCHING_CONCESSION: register-pinning
int WirelessController_BuildPeerRosterPacket(struct WirelessController *p)
{
    u8 *packet = p->peerRoster;
    u16 fill;
    u16 *fillPtr = &fill;
    int zero = 0;
    int i;
    asm("" : "+r"(zero));
    *fillPtr = zero;
    CpuSet(fillPtr, packet, 0x01000020);
    if (p->handshakeFlags & 0x100) {
        packet[0] = 1;
        {
            int bits = p->handshakeFlags;
            bits |= 8;
            bits |= zero;
            {
                int mask = 0xFEFF;
                p->handshakeFlags = bits & mask;
            }
        }
    } else
        packet[0] = zero;
    {
        u16 bits = p->handshakeFlags & 0x200;
        if (bits) {
            packet[1] = 1;
            {
                int flags = p->handshakeFlags | 16;
                int mask = 0xFDFF;
                p->handshakeFlags = flags & mask;
            }
        } else
            packet[1] = bits;
    }
    packet[3] = 1;
    for (i = 0; i <= 3; i++)
        if ((p->connectedSlots >> i) & 1)
            packet[3]++;
    packet[2] = (p->connectedSlots << 1) | 1;
    sub_08221380(gUnk_03006A80->my.uname, packet + 4, 8);
    *(u16 *)(packet + 0x36) = gUnk_03006A80->my.id;
    for (i = 0; i <= 3; i++) {
        int selected = (p->connectedSlots >> i) & 1;
        int next = i + 1;
        if (selected) {
            sub_08221380(gUnk_03006A80->partner[i].uname, packet + 14 + i * 10, 8);
            {
                int offset = next << 1;
                u16 *ids;
                asm("" : : "r"(offset));
                ids = (u16 *)(packet + 0x36);
                ids += next;
                *ids = gUnk_03006A80->partner[i].id;
            }
        }
    }
}

// MATCHING_CONCESSION: register-pinning
int WirelessController_UpdatePeerRosterSend(struct WirelessController *p)
{
    int result = 0;
    register int bits asm("r2") = p->handshakeFlags;
    if (bits & 2)
        goto fail;
    if (gUnk_03006A80->linkLossSlotFlag)
        goto fail;
    if (bits & 0xA0)
        goto fail;
    goto run;
fail:
    return 0;
run:
    if (p->operation == 9 && !(bits & 0x210))
        p->handshakeFlags = 0x205 | bits;
    {
        register int value asm("r0") = p->connectedSlots;
        int previous = p->previousConnectedSlots;
        int current;
        asm("" : "+r"(value), "+r"(previous));
        current = value;
        if (previous != current) {
            {
                int flags = p->handshakeFlags;
                int mask = 4;
                int unusedZero = 0;
                p->handshakeFlags = flags | mask;
            }
            if (gUnk_03006A80->connSlotFlag)
                p->handshakeFlags |= 1;
        }
        p->previousConnectedSlots = current;
    }
    if (p->handshakeFlags & 4) {
        WirelessController_BuildPeerRosterPacket(p);
        p->handshakeFlags &= ~4;
        result = 1;
    }
    {
        register int flags asm("r1") = p->handshakeFlags;
        int one = 1;
        if (one & flags) {
            if (!rfu_NI_setSendData(p->connectedSlots, 67, p->peerRoster, 64)) {
                int slot = 0;
                if (!(gUnk_03006A80->connSlotFlag & one)) {
                    do {
                        slot++;
                        if (slot > 3)
                            break;
                    } while (!((gUnk_03006A80->connSlotFlag >> slot) & 1));
                }
                p->sendSlot = slot;
                p->handshakeFlags = (p->handshakeFlags & ~1) | 2;
            } else
                Wireless_SetStatus(2);
        }
    }
    return result;
}

// The original caller saw an int-width prototype for rfu_clearSlot (u8, u8 in

// the SDK header): its argument order only matches with int parameters.

// MATCHING_CONCESSION: register-pinning
void WirelessController_PollPeerRosterSend(struct WirelessController *p)
{
    int changed = 0;
    if (p->handshakeFlags & 2) {
        struct RfuSlotStatusNI **slots = gUnk_03006A70;
        int slot = p->sendSlot;
        struct NIComm *s = &slots[slot]->send;
        {
            int state;
            state = s->state;
            if (state == 0x26) {
                register int original asm("r1") = s->bmSlotOrg;
                register int current asm("r0") = s->bmSlot;
                changed = current ^ original;
                rfu_clearSlot(4, slot);
                p->handshakeFlags &= ~2;
                if (p->handshakeFlags & 16)
                    p->handshakeFlags |= 128;
                if (p->handshakeFlags & 8)
                    p->handshakeFlags = (p->handshakeFlags & ~8) | 32;
            } else if (state == 0x27) {
                register int original asm("r1") = s->bmSlotOrg;
                register int current asm("r0") = s->bmSlot;
                changed = current ^ original;
                rfu_clearSlot(4, slot);
                {
                    int zero = 0;
                    p->handshakeFlags = zero;
                    if (p->operation == 9) {
                        int op = 7;
                        int byteZero = 0;
                        p->advanceRequested = byteZero;
                        p->operation = op;
                        p->operationTicks = zero;
                        p->operationEntered = 1;
                    }
                }
            }
        }
        if (changed)
            p->lostSlots |= changed;
    }
}

static inline void reset(struct WirelessController *p, int operation, int zero)
{
    p->advanceRequested = zero;
    p->operation = operation;
    p->operationTicks = zero;
    p->operationEntered = 1;
}

// MATCHING_CONCESSION: register-pinning
void WirelessController_OnLinkEvent(u8 command)
{
    int key = command;
    struct WirelessController *p = gUnk_02000484;
    if (!p)
        goto done;
    if (key == 0x33)
        goto release;
    if (key > 0x33)
        goto high;
    if (key == 0x13)
        goto reset5;
    if (key > 0x13)
        goto middle;
    if (key == 0x10)
        goto done;
    if (key > 0x10)
        goto lowerOne;
    if (key == 0)
        goto begin;
    goto done;
lowerOne:
    if (key == 0x11)
        goto addBits;
    goto done;
middle:
    if (key == 0x30)
        goto release;
    if (key > 0x30)
        goto flags;
    if (key == 0x14)
        goto mode;
    goto done;
flags:
    if (key == 0x31)
        goto selectedAdd;
    if (key == 0x32)
        goto selectedRemove;
    goto done;
high:
    if (key == 0x44)
        goto reset30;
    if (key > 0x44)
        goto endCommands;
    if (key == 0x41)
        goto done;
    if (key > 0x41)
        goto highPair;
    if (key == 0x40)
        goto releaseOther;
    goto done;
highPair:
    if (key == 0x42)
        goto reset32;
    if (key == 0x43)
        goto reset5Other;
    goto done;
endCommands:
    if (key == 0xF1)
        goto checkStop;
    if (key > 0xF1)
        goto lastCommands;
    if (key == 0x50)
        goto inactive;
    if (key == 0xF0)
        goto checkStop;
    goto done;
lastCommands:
    if (key == 0xF2)
        goto checkStop;
    if (key == 0xFF)
        goto stopIfRunning;
    goto done;

begin:
    reset(p, 5, key);
    goto done;
addBits: {
    register int incoming asm("r1") = gUnk_03005390.bytes[0x14];
    p->connectedSlots = p->connectedSlots | incoming;
}
    goto done;
reset5:
    reset(p, 5, 0);
    goto done;
mode: {
    int mode = gUnk_03006A80->connCount;
    if (!mode)
        reset(p, 5, mode);
    else
        reset(p, 9, 0);
}
    goto done;
selectedAdd: {
    u8 *slot = &p->selectedPeer;
    register int incoming asm("r1");
    incoming = gUnk_03005390.bytes[0x14];
    *slot = *slot | incoming;
}
    goto done;
selectedRemove: {
    u8 *slot = &p->selectedPeer;
    int incoming;
    incoming = gUnk_03005390.bytes[0x14];
    *slot = *slot & ~incoming;
}
    goto done;
release: {
    register union WirelessControllerEvent *message asm("r2") = &gUnk_03005390;
    u8 *selected;
    int incoming;
    incoming = message->bytes[0x14];
    p->connectedSlots &= ~incoming;
    selected = &p->selectedPeer;
    incoming = message->bytes[0x14];
    *selected &= ~incoming;
    {
        int zero = p->connectedSlots;
        if (!zero) {
            int op = 11;
            p->advanceRequested = zero;
            p->operation = op;
            p->operationTicks = zero;
            p->operationEntered = 1;
        }
    }
}
    goto done;
releaseOther: {
    register union WirelessControllerEvent *message asm("r2") = &gUnk_03005390;
    u8 *selected;
    int incoming;
    incoming = message->bytes[0x14];
    p->connectedSlots &= ~incoming;
    selected = &p->selectedPeer;
    incoming = message->bytes[0x14];
    *selected &= ~incoming;
    {
        int zero = p->connectedSlots;
        if (!zero) {
            int op = 11;
            p->advanceRequested = zero;
            p->operation = op;
            p->operationTicks = zero;
            p->operationEntered = 1;
        }
    }
}
    goto done;
reset32:
    reset(p, 32, 0);
    goto done;
reset5Other:
    reset(p, 5, 0);
    goto done;
reset30:
    reset(p, 30, 0);
    goto done;
stopIfRunning:
    if (p->operation == 28 || p->operation <= 1)
        goto inactive;
    goto stop;
checkStop:
    if (p->operation == 28)
        goto inactive;
stop:
    reset(p, 28, 0);
inactive:
    gUnk_020000EC = 0;
    goto done;
done:;
}

#define Wireless_OnChildConnectionEvent sub_080347D8

void RfuManager_SendData(u32);

int sub_082238F0(int);

static inline void childOperation(struct WirelessController *p, int op, int zero)
{
    p->advanceRequested = zero;
    p->operation = op;
    p->operationTicks = zero;
    p->operationEntered = 1;
}

void Wireless_OnChildConnectionEvent(u8 command)
{
    int key = command;
    struct WirelessController *p = gUnk_02000484;
    if (!p)
        goto done;
    if (key == 0x33)
        goto lost;
    if (key > 0x33)
        goto high;
    if (key == 0x23)
        goto reset6a;
    if (key > 0x23)
        goto middle;
    if (key == 0x20)
        goto available;
    if (key > 0x20)
        goto pair;
    if (key == 0)
        goto begin;
    goto done;
pair:
    if (key == 0x21)
        goto reset6;
    if (key == 0x22)
        goto connected;
    goto done;
middle:
    if (key == 0x30)
        goto lost;
    if (key > 0x30)
        goto flags;
    if (key == 0x24)
        goto count;
    if (key == 0x25)
        goto reset6b;
    goto done;
flags:
    if (key == 0x31)
        goto selectedAdd;
    if (key == 0x32)
        goto selectedRemove;
    goto done;
high:
    if (key == 0x45)
        goto done;
    if (key > 0x45)
        goto endCommands;
    if (key == 0x42)
        goto reset32;
    if (key > 0x42)
        goto highPair;
    if (key == 0x40)
        goto release;
    goto done;
highPair:
    if (key == 0x43)
        goto reset6c;
    if (key == 0x44)
        goto reset30;
    goto done;
endCommands:
    if (key == 0xF1)
        goto checkStop;
    if (key > 0xF1)
        goto lastCommands;
    if (key == 0x50)
        goto send;
    if (key == 0xF0)
        goto checkStop;
    goto done;
lastCommands:
    if (key == 0xF2)
        goto stop;
    if (key == 0xFF)
        goto stopIfRunning;
    goto done;
send:
    RfuManager_SendData(1);
    goto done;
begin:
    childOperation(p, 6, key);
    goto done;
available:
    p->availableSlots = gUnk_03005390.fields.code;
    goto done;
reset6:
    childOperation(p, 6, 0);
    goto done;
connected: {
    int incoming = gUnk_03005390.fields.code;
    u8 *slot = &p->parentSlot;
    int zero = 0;
    *slot = incoming;
    rfu_setRecvBuffer(32, *slot, p->peerRoster, 64);
    childOperation(p, 14, zero);
}
    goto done;
reset6a:
    childOperation(p, 6, 0);
    goto done;
count: {
    u8 *count = &p->unk43;
    int n;
    *count = 0;
    n = gUnk_03006A80->connSlotFlag;
    n--;
    if (n) {
        do {
            (*count)++;
        } while (--n);
    }
}
    goto done;
reset6b:
    childOperation(p, 6, 0);
    goto done;
selectedAdd: {
    u8 *selected = &p->selectedPeer;
    int incoming = gUnk_03005390.bytes[0x14];
    int zero;
    {
        int current = *selected | incoming;
        zero = 0;
        *selected = current;
    }
    p->operationTimer = p->operation;
    childOperation(p, 16, zero);
}
    goto done;
selectedRemove: {
    u8 *selected = &p->selectedPeer;
    int incoming = gUnk_03005390.bytes[0x14];
    int zero;
    {
        int current = *selected & ~incoming;
        zero = 0;
        *selected = current;
    }
    childOperation(p, p->operationTimer, zero);
}
    goto done;
lost: {
    u8 *selected = &p->selectedPeer;
    int incoming = gUnk_03005390.bytes[0x14];
    int zero;
    {
        int current = *selected & ~incoming;
        zero = 0;
        *selected = current;
    }
    if (p->operation > 15) {
        childOperation(p, 29, zero);
        sub_082238F0(1);
        goto done;
    }
    childOperation(p, 11, zero);
}
    goto done;
release:
    childOperation(p, 11, 0);
    p->selectedPeer &= ~gUnk_03005390.bytes[0x14];
    goto done;
reset32:
    childOperation(p, 32, 0);
    goto done;
reset6c:
    childOperation(p, 6, 0);
    goto done;
reset30:
    childOperation(p, 30, 0);
    goto done;
checkStop:
    if (p->operation == 28)
        goto done;
    goto stop;
stopIfRunning:
    if (p->operation == 28 || p->operation <= 1)
        goto done;
stop:
    sub_082238F0(1);
    childOperation(p, 28, 0);
done:;
}

void WirelessController_OnReceiveRequest(void)
{
    rfu_REQ_recvData();
}

/* WirelessController_RequestReceiveIfCreated: Issues an RFU receive request when the controller exists */

#define WirelessController_RequestReceiveIfCreated sub_080349FC

void sub_080349FC(void)
{
    if (gUnk_02000484)
        rfu_REQ_recvData();
}

u32 WirelessController_ClearOperationEntryFlag(struct WirelessController *p)
{
    u32 v;
    u8 *q = &p->operationEntered;
    v = *q;
    if (v)
        *q = 0;
}

static inline void setOperation(struct WirelessController *p, int op, int zero)
{
    p->advanceRequested = zero;
    p->operation = op;
    p->operationTicks = zero;
    p->operationEntered = 1;
}

int RfuManager_Init(void *, void *);

int WirelessController_InitLinkCallbacks(struct WirelessController *p)
{
    if (p->operationEntered)
        p->operationEntered = 0;
    {
        int status = (u8)RfuManager_Init(WirelessController_OnLinkEvent,
                                         WirelessController_OnReceiveRequest);
        if (status)
            setOperation(p, 27, 0);
        else {
            setOperation(p, 2, status);
            p->adapterInitialized = 1;
        }
    }
    return 0;
}

int Wireless_CheckAdapterReady(void);

int SunClock_GetPhase(void);

int SunClock_GetBaseHour(void);

void WirelessText_EncodeString(const void *, void *, int);

void sub_08224DAC(void *, int, int, int, int, const void *, const void *);

// MATCHING_CONCESSION: register-pinning
int WirelessController_UpdateAdapterSetup(struct WirelessController *p)
{
    u8 *pending = &p->operationEntered;
    u8 *timer;
    register int read asm("r0");
    int zero;
    if (*pending)
        *pending = 0;
    timer = &p->setupDelay;
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
            const u16 *table;
            register u32 mask asm("r4");
            u32 *randomIndex;
            int offset;
            int chance;
            const u8 *name;
            u8 *userName;
            u8 *gameName;
            p->advanceRequested = zero;
            p->operation = three;
            p->operationTicks = zero;
            *pending = 1;
            base = Script_ParseStringRef(p->identityScript);
            table = gUnk_0203B400;
            randomIndex = &gUnk_03005308;
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
            WirelessText_EncodeString(gUnk_02000710->playerName, p->userName, 8);
            userName = p->userName;
            gameName = p->gameName;
            WirelessText_EncodeString(name, gameName, 11);
            p->gameIdLow = p->gameId;
            p->gameIdHigh = p->gameId >> 8;
            {
                void *config = (p->linkConfiguration);
                int mode = p->active;
                int flags = 0;
                if (mode)
                    flags = 3;
                sub_08224DAC(config, mode, 43, flags, 0, gameName, userName);
            }
            break;
        }
        default:
            break;
    }
done:
    return 0;
}

void RfuManager_StartConfiguration(void *);

int WirelessController_StartConfiguredLink(struct WirelessController *p)
{
    if (p->operationEntered)
        p->operationEntered = 0;
    if (p->operationTicks > 59) {
        RfuManager_StartConfiguration((p->linkConfiguration));
        gUnk_020000EC = 0;
        setOperation(p, 4, 0);
    }
    return 0;
}

u32 Wireless_ClearIdleEntry(struct WirelessController *p)
{
    u8 *q = &p->operationEntered;
    if (*q)
        *q = 0;
    return 0;
}

int sub_082236C8(int, int, int, const u8 *);

extern const u8 gUnk_0824DE60[];

// MATCHING_CONCESSION: register-pinning
int Wireless_StartChildSearch(struct WirelessController *p)
{
    int raw;
    int status;
    int result;
    if (p->operationEntered) {
        int i;
        void *buffer;
        p->operationEntered = 0;
        i = 0;
        buffer = p->handshakeWork;
        do {
            rfu_setRecvBuffer(32, (u8)i, buffer, 4);
            i++;
        } while (i <= 3);
    }
    raw = sub_082236C8(1, 0, 600, gUnk_0824DE60);
    {
        register int narrowed asm("r1") = raw;
        asm("" : "+r"(narrowed));
        status = (u8)narrowed;
    }
    if (!status)
        goto success;
    setOperation(p, 27, 0);
    result = -1;
    goto done;
success: {
    register int operation asm("r1") = 7;
    register int one asm("r1");
    p->advanceRequested = status;
    p->operation = operation;
    p->operationTicks = status;
    {
        register u8 *pending asm("r2") = &p->operationEntered;
        one = 1;
        *pending = one;
    }
}
    result = raw;
done:
    return result;
}

int Wireless_StartParentSearch(struct WirelessController *p)
{
    int raw;
    int status;
    int result;
    if (p->operationEntered) {
        p->operationEntered = 0;
        p->availableSlots = 0;
    }
    raw = sub_082236C8(0, 0, 600, gUnk_0824DE60);
    {
        int narrowed = raw;
        status = (u8)narrowed;
    }
    if (!status) {
        goto success;
    }
    setOperation(p, 27, 0);
    result = -1;
    goto done;
    result = raw;
done:
    return result;

success: {
    int operation = 8;
    int one;
    p->advanceRequested = status;
    p->operation = operation;
    p->operationTicks = status;
    one = 1;
    p->operationEntered = one;
}
}

// MATCHING_CONCESSION: register-pinning
int Wireless_UpdatePeerHandshake(struct WirelessController *p)
{
    int bits;
    if (p->operationEntered) {
        p->operationEntered = 0;
        p->peerRefreshRequested = 0;
        p->advanceRequested = 0;
    }
    if (WirelessController_ConsumePeerRefreshRequest(p)) {
        if (!p->peerRoster[1] && !(p->handshakeFlags & 0x238))
            p->handshakeFlags = 0x105 | p->handshakeFlags;
        p->peerRefreshRequested = 0;
    }
    bits = p->handshakeFlags;
    if (!(bits & 64))
        goto receive;
    if (bits & 0x338)
        goto receive;
    {
        register int literal asm("r3") = 0x105;
        register int value asm("r1");
        asm("" : "+r"(literal));
        value = literal;
        value |= bits;
        p->handshakeFlags = value;
    }
receive:;
    if (gUnk_03006A70[p->sendSlot]->recv.state == 0x46) {
        /* Keep base-plus-slot before the field offset: typed indexing reverses these adds. */
        u8 *base = (u8 *)p + p->sendSlot * 4;
        u8 *message;
        message = base + offsetof(struct WirelessController, handshakeWork);
        if (*message) {
            *message = 0;
            p->handshakeFlags |= 64;
        }
        rfu_clearSlot(8, p->sendSlot);
    }
    bits = p->handshakeFlags;
    if ((bits & 96) == 96) {
        int mask = 0xFFDF;
        int second;
        mask &= bits;
        second = 0xFFBF;
        mask &= second;
        p->handshakeFlags = mask;
        p->lostSlots |= gUnk_03006A80->connSlotFlag | gUnk_03006A80->linkLossSlotFlag;
        return 0;
    } else {
        int connected = p->connectedSlots;
        if (connected) {
            int blocked = bits & 0x168;
            if (!blocked && connected == gUnk_03006A80->connSlotFlag) {
                if (WirelessController_ConsumeAdvanceRequest(p)) {
                    setOperation(p, 9, blocked);
                    return 0;
                }
            }
        }
    }
}

int sub_082237A0(int, int);

extern u16 gUnk_03005260[];

// MATCHING_CONCESSION: register-pinning
int Wireless_SelectParentFromInput(struct WirelessController *p)
{
    u8 *count;
    if (p->operationEntered)
        p->operationEntered = 0;
    WirelessController_RebuildAvailablePeerList(p);
    {
        u8 *initial = &p->peerCount;
        int available = *initial;
        {
            u8 *savedCount = initial;
            count = savedCount;
        }
        if (available) {
            s8 *scroll = &p->peerScroll;
            int old = *scroll;
            register u16 *buttons asm("r2") = gUnk_03005260;
            register int pressed asm("r5");
            int current;
            int index;
            int n;
            pressed = buttons[1];
            if (pressed & 64)
                (*scroll)--;
            else if (pressed & 128)
                (*scroll)++;
            current = *scroll;
            if (old != current) {
                if (current > 7)
                    *scroll = 7;
                else if (current < 0)
                    *scroll = 0;
            }
            index = (s8)(u8)*scroll;
            n = *count;
            if ((buttons[1] & 1) && index < n) {
                int parentIndex = p->peerIds[index];
                struct RfuLinkStatus *link;
                int status;
                link = gUnk_03006A80;
                status = (u8)sub_082237A0(link->partner[parentIndex].id, 0);
                if (!status) {
                    setOperation(p, 10, status);
                    return 0;
                }
            }
        }
    }
}

void InterruptsAndVideo_WaitFrames(int);

int Wireless_WaitForParentTransfer(struct WirelessController *p)
{
    if (p->operationEntered) {
        p->operationEntered = 0;
        Sound_PlaySong(221);
    }
    if (p->handshakeFlags & 128) {
        InterruptsAndVideo_WaitFrames(8);
        Wireless_BeginParentNiSession(p);
    }
}

int Wireless_UpdateDisconnectTimeout(struct WirelessController *p)
{
    if (p->operationEntered)
        p->operationEntered = 0;
    if (p->operationTicks > 599) {
        sub_082238F0(0);
        setOperation(p, 4, 0);
    }
}

int Wireless_UpdateSearchRestartDelay(struct WirelessController *p)
{
    if (p->operationEntered) {
        p->operationEntered = 0;
        Sound_PlaySong(222);
    }
    if (p->operationTicks > 89) {
        WirelessController_ResetPeerExchange(p);
        {
            int active = p->active;
            if (active)
                setOperation(p, 7, 0);
            else
                setOperation(p, 6, active);
        }
    }
}

int Wireless_DisconnectAllSlots(struct WirelessController *p)
{
    if (p->operationEntered)
        p->operationEntered = 0;
    if ((u8)rfu_getMasterSlave() == 1) {
        rfu_REQ_disconnect(gUnk_03006A80->connSlotFlag | gUnk_03006A80->linkLossSlotFlag);
        rfu_waitREQComplete();
    }
}

int Wireless_UpdateParentSearchDelay(struct WirelessController *p)
{
    u8 *pending = &p->operationEntered;
    if (*pending)
        *pending = 0;
    if (p->operationTicks > 120)
        setOperation(p, 8, 0);
}

// The original caller saw an int-width prototype for rfu_clearSlot (u8, u8 in

// the SDK header): its argument order only matches with int parameters.

void RfuManager_RequestMscCycleCompletion(void);

static inline void requestUpdate(struct WirelessController *p)
{
    int flags = p->handshakeFlags;
    register int mask asm("r3") = 256;
    register int value asm("r2");
    asm("" : "+r"(flags), "+r"(mask));
    value = mask;
    value |= flags;
    value |= 1;
    asm("" : "+r"(value));
    p->handshakeFlags = value;
}

static inline void requestUpdateB(struct WirelessController *p, int flags)
{
    register int mask asm("r3") = 256;
    register int value asm("r1");
    asm("" : "+r"(mask));
    value = mask;
    value |= flags;
    value |= 1;
    asm("" : "+r"(value));
    p->handshakeFlags = value;
}

static inline void finish(struct WirelessController *p, int operation, u8 byteZero, u32 wordZero)
{
    p->advanceRequested = byteZero;
    p->operation = operation;
    p->operationTicks = wordZero;
    p->operationEntered = 1;
}

// MATCHING_CONCESSION: register-pinning
int Wireless_UpdateChildHandshake(struct WirelessController *p)
{
    register u8 *slot asm("r5");
    int flags;
    if (p->operationEntered)
        p->operationEntered = 0;
    {
        struct RfuSlotStatusNI **slots = gUnk_03006A70;
        register u8 *initial asm("r3") = &p->parentSlot;
        int state;
        state = slots[*initial]->recv.state;
        slot = initial;
        if (state == 0x46) {
            if (p->peerRoster[1])
                Wireless_BeginChildTransfer(p);
            {
                register u32 offset asm("r3") = 0x134;
                u8 *marker;
                asm("" : "+r"(offset));
                marker = (u8 *)p + offset;
                if (*marker) {
                    *marker = 0;
                    p->handshakeFlags |= 64;
                }
            }
            rfu_clearSlot(8, *slot);
        }
    }
    if (gUnk_03005260[1] & 2)
        requestUpdate(p);
    flags = p->handshakeFlags;
    if (!(flags & 64))
        goto send;
    if (flags & 40)
        goto send;
    requestUpdateB(p, flags);
send:;
    flags = p->handshakeFlags;
    if (!(flags & 2)) {
        if (!gUnk_03006A80->linkLossSlotFlag) {
            u8 *data;
            int one = 1;
            asm("" : "+r"(one));
            {
                int test = one & flags;
                if (!test)
                    goto maybeComplete;
                {
                    int cancel = 256 & flags;
                    asm("" : "+r"(cancel));
                    data = (u8 *)&p->handshakeNumber;
                    if (cancel) {
                        int bits;
                        int mask;
                        *data = 1;
                        bits = p->handshakeFlags;
                        mask = 0xFEFF;
                        mask &= bits;
                        mask |= 8;
                        p->handshakeFlags = mask;
                    }
                    rfu_clearSlot(4, *slot);
                    {
                        register int sendMask asm("r0") = one;
                        asm("" : "+r"(sendMask));
                        if (rfu_NI_setSendData((u8)(sendMask << *slot), 6, data, 4)) {
                            Wireless_SetStatus(2);
                            return 0;
                        }
                        p->handshakeFlags = (p->handshakeFlags & ~1) | 2;
                    }
                }
            }
        }
    maybeComplete:
        if (!(p->handshakeFlags & 2))
            goto complete;
    }
    {
        struct RfuSlotStatusNI **slots = gUnk_03006A70;
        int index;
        int state;
        index = *slot;
        state = slots[index]->send.state;
        if (state == 0x26) {
            rfu_clearSlot(4, index);
            p->handshakeFlags &= ~2;
            if (p->handshakeFlags & 8) {
                p->handshakeFlags = (p->handshakeFlags & ~8) | 32;
                *(u8 *)&p->handshakeNumber = 0;
            }
        } else if (state == 0x27) {
            rfu_clearSlot(4, index);
            p->handshakeFlags = 0;
        }
    }
complete:
    flags = p->handshakeFlags;
    if ((flags & 96) == 96) {
        int value = 0xFFDF;
        register u8 zeroByte asm("r4");
        u32 zeroWord;
        register int second asm("r1");
        value &= flags;
        zeroByte = 0;
        zeroWord = 0;
        asm("" : "+r"(zeroByte), "+r"(zeroWord));
        second = 0xFFBF;
        value &= second;
        p->handshakeFlags = value;
        RfuManager_RequestMscCycleCompletion();
        finish(p, 12, zeroByte, zeroWord);
        return 0;
    }
}

u32 Wireless_ClearFirstHandshakeWaitEntry(struct WirelessController *p)
{
    u32 v;
    u8 *q = &p->operationEntered;
    v = *q;
    if (v)
        *q = 0;
}

u32 Wireless_ClearSecondHandshakeWaitEntry(struct WirelessController *p)
{
    u32 v;
    u8 *q = &p->operationEntered;
    v = *q;
    if (v)
        *q = 0;
}

u32 Wireless_ClearThirdHandshakeWaitEntry(struct WirelessController *p)
{
    u32 v;
    u8 *q = &p->operationEntered;
    v = *q;
    if (v)
        *q = 0;
}

u32 Wireless_ClearFourthHandshakeWaitEntry(struct WirelessController *p)
{
    u32 v;
    u8 *q = &p->operationEntered;
    v = *q;
    if (v)
        *q = 0;
}

int Wireless_WaitForReset(struct WirelessController *p)
{
    if (p->operationEntered) {
        p->operationEntered = 0;
        p->peerRefreshRequested = 0;
        p->advanceRequested = 0;
    }
    if (WirelessController_ConsumeAdvanceRequest(p))
        WirelessController_ResetSession(p);
    return 0;
}

int Wireless_ResetAfterError(struct WirelessController *p)
{
    if (p->operationEntered) {
        int zero = 0;
        p->operationEntered = zero;
        Wireless_SetStatus(1);
        sub_082238F0(1);
        p->peerRefreshRequested = zero;
        p->advanceRequested = zero;
    }
    if (WirelessController_ConsumeAdvanceRequest(p))
        WirelessController_ResetSession(p);
    return 0;
}

int Wireless_HandleSendError(struct WirelessController *p)
{
    u8 *pending = &p->operationEntered;
    if (*pending) {
        int zero = 0;
        *pending = zero;
        Wireless_SetStatus(2);
        sub_082238F0(1);
        p->peerRefreshRequested = zero;
        p->advanceRequested = zero;
    }
    if (WirelessController_ConsumeAdvanceRequest(p)) {
        int operation = 30;
        int zero = 0;
        p->advanceRequested = zero;
        p->operation = operation;
        p->operationTicks = zero;
        *pending = 1;
    }
    return 0;
}

void RfuManager_RequestStopMode(void);

u32 Wireless_RequestAdapterStop(struct WirelessController *p)
{
    if (p->operationEntered) {
        p->operationEntered = 0;
        RfuManager_RequestStopMode();
    }
    return 0;
}

int Wireless_RequestDisconnect(struct WirelessController *p)
{
    if (p->operationEntered) {
        p->operationEntered = 0;
        sub_082238F0(p->disconnectMode);
    }
    return 0;
}

u32 Wireless_ResetControllerAndSlots(struct WirelessController *p)
{
    rfu_clearAllSlot();
    WirelessController_ResetSession(p);
}

void Wireless_UpdateUniAcknowledgedSlots(struct WirelessController *p)
{
    u8 *seen = (&p->acknowledgedSlots);
    int bits = (gUnk_03005390.bytes[3] << 1) | *seen;
    int zero = 0;
    int offset;
    int expected;
    *seen = bits;
    offset = (offsetof(struct WirelessController, peerRoster) + 2);
    {
        u8 *required = (u8 *)p + offset;
        expected = *required;
    }
    if ((bits & expected) == expected) {
        u8 *waiting = (&p->awaitingAck);
        *waiting = zero;
        p->sendReady = zero;
    } else {
        u8 *waiting = (&p->awaitingAck);
        *waiting = 1;
        (*(u32 *)(((u8 *)&p->ackWaitFrames)))++;
    }
}

void Wireless_EnqueuePacket(void *, u8 *);

// MATCHING_CONCESSION: register-pinning
int Wireless_ReceiveChildUniPacket(struct WirelessController *p)
{
    u8 *scratch = p->peerRoster;
    int result = 0;
    int slot;
    {
        struct RfuSlotStatusUNI **slots = gUnk_03006A60;
        slot = p->parentSlot;
        if (slots[slot]->recv.newDataFlag) {
            u8 *packet = (p->childUniPacket);
            u8 *previous;
            u8 *sequence;
            register int last asm("r6");
            int next;
            int received;
            int i;
            u16 *flags;
            u8 *changed;
            rfu_UNI_clearRecvNewDataFlag(slot);
            {
                register u8 *initial asm("r0") = (p->previousSequence);
                u8 *incoming;
                last = *initial;
                next = last + 1;
                incoming = (p->childUniPacket + 4);
                next = (u8)next;
                received = *incoming;
                previous = initial;
                sequence = incoming;
            }
            if (next == received) {
                Wireless_EnqueuePacket(p->packetQueue, sequence);
            } else {
                register int old asm("r0") = (u8)last;
                register u8 *counter asm("r1");
                if (old == received)
                    counter = (p->duplicatePackets);
                else
                    counter = (p->stalePackets);
                (*counter)++;
            }
            {
                int value = *sequence;
                int zero = 0;
                *previous = value;
                (*packet)++;
                packet[1] = zero;
                i = zero;
                flags = &p->localInput;
                changed = &p->inputChanged;
            }
            do {
                register u8 *read asm("r6") = scratch;
                if ((read[2] >> i) & 1)
                    packet[1]++;
            } while (++i <= 4);
            {
                int value = *flags;
                register int zero asm("r1") = 0;
                *(u16 *)(packet + 2) = value;
                value = 65535;
                *flags = value;
                *changed = zero;
            }
            result = 1;
        }
    }
    return result;
}

void Wireless_ProcessChildUniExchange(struct WirelessController *p)
{
    if (Wireless_ReceiveChildUniPacket(p))
        rfu_UNI_readySendData(p->parentSlot);
    RfuManager_SendData(1);
}

void Wireless_ResetRemoteInputHistory(struct WirelessController *p)
{
    u16 *history = (u16 *)(((u8 *)&p->previousInput));
    int bits;
    int i;
    {
        int initial = 65535;
        *history++ = initial;
        initial = -1;
        *history = initial;
        bits = initial;
    }
    history += 2;
    i = 4;
    do {
        int value = *history;
        value |= bits;
        *history++ = value;
    } while (--i >= 0);
}

// MATCHING_CONCESSION: register-pinning
void Wireless_ReceiveParentUniPackets(struct WirelessController *argument)
{
    struct WirelessController *p = argument;
    u8 *scratch = p->peerRoster;
    int i;
    u8 *packet;
    register u8 *seen asm("r8");
    p->inputBlocked = 0;
    i = 0;
    packet = (p->parentUniReceive);
    do {
        int offset = i << 2;
        int bit = 2 << i;
        int test;
        u8 *initialSeen;
        {
            u8 *read = scratch;
            test = read[2] & bit;
        }
        {
            register int offset asm("r3") = 0xE6;
            asm("" : "+r"(offset));
            initialSeen = (u8 *)(offset + (u32)p);
        }
        seen = initialSeen;
        if (test) {
            {
                struct RfuSlotStatusUNI **entry =
                    (struct RfuSlotStatusUNI **)(offset + (u32)gUnk_03006A60);
                if ((*entry)->recv.newDataFlag) {
                    u8 *previous;
                    int old;
                    int incoming;
                    int next;
                    register int zero asm("r9");
                    bit |= *initialSeen;
                    zero = 0;
                    *initialSeen = bit;
                    rfu_UNI_clearRecvNewDataFlag(i);
                    {
                        register u8 *initial asm("r1") = (p->previousSequence);
                        old = initial[i];
                        next = (u8)(old + 1);
                        incoming = packet[0];
                        previous = initial;
                    }
                    if (next != incoming) {
                        {
                            int previousValue = old;
                            asm("" : "+r"(previousValue));
                            if (previousValue == incoming) {
                                {
                                    u8 *counter = (p->duplicatePackets);
                                    counter += i;
                                    (*counter)++;
                                }
                                {
                                    register int value asm("r2") = zero;
                                    *(u16 *)(packet + 2) = value;
                                }
                            } else if ((u32)previousValue < (u32)incoming) {
                                {
                                    u8 *counter = (p->missedPackets);
                                    counter += i;
                                    (*counter)++;
                                }
                                {
                                    register int value asm("r3") = zero;
                                    *(u16 *)(packet + 2) = value;
                                }
                            } else {
                                {
                                    u8 *counter = (p->stalePackets);
                                    counter += i;
                                    (*counter)++;
                                }
                            }
                        }
                    }
                    previous[i] = *packet;
                }
            }
        }
        packet += 4;
    } while (++i <= 3);
    {
        int got = *seen;
        int expected;
        register int saved asm("r3");
        register int matched asm("r2");
        u8 *sent;
        {
            u8 *read = scratch;
            expected = read[2];
        }
        saved = expected;
        matched = saved;
        matched &= got;
        got = expected;
        sent = (&p->acknowledgedSlots);
        if (matched == got) {
            saved &= *sent;
            if (saved == matched)
                goto checkSent;
            {
                u8 *counter = (&p->receiveAheadCount);
                (*counter)++;
            }
        }
        {
            u8 *read;
            int value = *sent;
            read = scratch;
            got = read[2];
            value &= got;
            if (value != got)
                return;
        }
    checkSent: {
        register u8 *read asm("r2") = seen;
        int value;
        value = *read;
        value &= got;
        if (value != got) {
            u8 *counter = (&p->sendAheadCount);
            (*counter)++;
        }
    }
    }
}

// MATCHING_CONCESSION: register-pinning
void Wireless_BuildParentUniPacket(struct WirelessController *argument)
{
    struct {
        u32 fill;
        int sequence;
        u8 *waiting;
        u8 *sent;
    } stack;
    register struct WirelessController *p asm("r5") = argument;
    u8 *scratch = p->peerRoster;
    u8 *packet = (p->parentUniPacket);
    u8 *sent;
    int got;
    int expected;
    u8 *read;
    {
        int offset = offsetof(struct WirelessController, acknowledgedSlots);
        register u8 *initial asm("r1");
        asm("" : "+r"(offset));
        initial = (u8 *)(offset + (u32)p);
        sent = initial;
        got = *initial;
        read = scratch;
        expected = read[2];
    }
    got &= expected;
    if (got == expected) {
        int zero = p->inputBlocked;
        if (zero) {
            if (read[3] > 1) {
                register u8 *waiting asm("r1") = &p->sendReady;
                *waiting = 1;
            }
        } else {
            u8 *waiting;
            int i;
            u16 *backup;
            u16 *flags;
            u8 *seen;
            int bit;
            register u16 *input asm("r3");
            u16 *output;
            stack.sequence = *packet;
            Wireless_EnqueuePacket(p->packetQueue, packet);
            waiting = &p->sendReady;
            *waiting = zero;
            stack.fill = zero;
            CpuSet(&stack.fill, packet, 0x05000004);
            i = 0;
            stack.sent = sent;
            stack.waiting = waiting;
            backup = (u16 *)(((u8 *)&p->previousInput));
            {
                int offset = offsetof(struct WirelessController, localInput);
                flags = (u16 *)(offset + (u32)p);
            }
            {
                int offset = offsetof(struct WirelessController, receivedSlots);
                seen = (u8 *)(offset + (u32)p);
            }
            {
                int initial = 2;
                bit = initial;
            }
            input = (u16 *)(((u8 *)p->parentUniReceive + 2));
            output = (u16 *)(waiting + 0x40);
            do {
                int mask = bit;
                u8 *read;
                mask <<= i;
                read = scratch;
                if (read[2] & mask)
                    *output = *input;
                input += 2;
                output++;
            } while (++i <= 3);
            *(u16 *)(packet + 4) = *backup;
            {
                u16 *read = flags;
                int value;
                value = *read;
                *backup = value;
                value = 65535;
                *read = value;
            }
            {
                u8 *read = scratch;
                packet[2] = read[2];
            }
            packet[1] = 1;
            i = 0;
            {
                register int sequence asm("r3") = stack.sequence + 1;
                register int bit asm("r5") = 2;
                do {
                    u8 *read = scratch;
                    int active;
                    int mask;
                    active = read[2];
                    mask = bit << i;
                    asm("" : "+r"(mask));
                    if (active & mask)
                        packet[1]++;
                } while (++i <= 3);
                *packet = sequence;
            }
            {
                int one = 1;
                register u8 *write asm("r2") = stack.sent;
                *write = one;
                {
                    register u8 *write asm("r5") = seen;
                    asm("" : "+r"(write));
                    *write = one;
                }
                {
                    u8 *read = scratch;
                    if (read[3] > 1) {
                        register u8 *write asm("r5") = stack.waiting;
                        *write = one;
                    }
                }
            }
        }
    }
}

void Wireless_RemoveConnectedSlots(struct WirelessController *p, u8 slots)
{
    u8 *scratch = p->peerRoster;
    int mask = (u8)(slots << 1);
    int i = 0;
    int active = scratch[2];
    do {
        if (((2 << i) & active) & mask) {
            if (scratch[3] > 1)
                scratch[3]--;
        }
    } while (++i <= 4);
    {
        int active = scratch[2] & ~mask;
        scratch[2] = active;
        if ((p->acknowledgedSlots & active) == active)
            p->awaitingAck = 0;
    }
}

// MATCHING_CONCESSION: register-pinning
void Wireless_OnParentUniSessionEvent(u8 command)
{
    register int key asm("r3") = command;
    int saved = key;
    struct WirelessController *p = gUnk_02000484;
    if (!p)
        goto done;
    if (key == 0x41)
        goto done;
    if (key > 0x41)
        goto high;
    if (key == 0x30)
        goto disconnected;
    if (key > 0x30)
        goto lowMore;
    goto done;
lowMore:
    if (key == 0x32)
        goto removed;
    /* Range switch retains the original strict comparison
                       boundary. */
    switch (key) {
        case 0x32 ... 0x7FFFFFFF:
            break;
        default:
            goto added;
    }
    if (key == 0x33)
        goto disconnected;
    if (key == 0x40)
        goto lost;
    goto done;
high:
    if (key == 0x50)
        goto receive;
    if (key > 0x50)
        goto last;
    if (key == 0x43)
        goto done;
    switch (key) {
        case 0x43 ... 0x7FFFFFFF:
            break;
        default:
            goto reset;
    }
    if (key == 0x44)
        goto stop;
    goto done;
last:
    if (key == 0xF1)
        goto error;
    if (key > 0xF1)
        goto lastMore;
    if (key == 0xF0)
        goto error;
    goto done;
lastMore:
    if (saved == 0xF2)
        goto error;
    if (saved == 0xFF)
        goto error;
    goto done;
receive:
    Wireless_UpdateUniAcknowledgedSlots(p);
    Wireless_ReceiveParentUniPackets(p);
    gUnk_020000EC = 0;
    goto done;
added: {
    u8 *slots = &p->selectedPeer;
    register int incoming asm("r1") = (u8)gUnk_03005390.fields.code;
    *slots |= incoming;
}
    goto done;
removed: {
    u8 *slots = &p->selectedPeer;
    int incoming = (u8)gUnk_03005390.fields.code;
    *slots &= ~incoming;
}
    goto done;
disconnected: {
    u8 *slots = &p->selectedPeer;
    union WirelessControllerEvent *event = &gUnk_03005390;
    int zero;
    int remaining;
    int incoming = (u8)event->fields.code;
    int selected = *slots & ~incoming;
    zero = 0;
    *slots = selected;
    p->slotsChanged = 1;
    Wireless_RemoveConnectedSlots(p, (u8)event->fields.code);
    remaining = p->peerRoster[3];
    if (remaining != 1)
        goto done;
    sub_082238F0(1);
    {
        int op = 29;
        p->advanceRequested = zero;
        p->operation = op;
        p->operationTicks = zero;
    }
    p->operationEntered = remaining;
}
    goto done;
lost: {
    int remaining;
    Wireless_RemoveConnectedSlots(p, (u8)gUnk_03005390.fields.code);
    remaining = p->peerRoster[3];
    if (remaining != 1)
        goto done;
    sub_082238F0(1);
    setOperation(p, 29, 0);
    p->operationEntered = remaining;
}
    goto done;
reset:
    WirelessController_ResetSession(p);
    goto done;
stop:
    setOperation(p, 30, 0);
    goto done;
error:
    sub_082238F0(1);
    setOperation(p, 28, 0);
    gUnk_020000EC = 0;
done:;
}

// MATCHING_CONCESSION: register-pinning
void Wireless_OnChildUniSessionEvent(u8 command)
{
    register int key asm("r3") = command;
    int saved = key;
    struct WirelessController *p = gUnk_02000484;
    if (!p)
        goto done;
    if (key > 0x41)
        goto high;
    /* Range switch retains the original strict comparison
                       boundary. */
    switch (key) {
        case 0x40 ... 0x7FFFFFFF:
            goto done;
        default:
            break;
    }
    if (key == 0x30)
        goto disconnected;
    if (key <= 0x30)
        goto done;
    if (key == 0x32)
        goto removed;
    switch (key) {
        case 0x32 ... 0x7FFFFFFF:
            break;
        default:
            goto added;
    }
    if (key == 0x33)
        goto disconnected;
    goto done;
high:
    if (key == 0x50)
        goto receive;
    if (key > 0x50)
        goto last;
    if (key == 0x42)
        goto reset;
    goto done;
last:
    if (key == 0xF1)
        goto error;
    if (key > 0xF1)
        goto lastMore;
    if (key == 0xF0)
        goto checkError;
    goto done;
lastMore:
    if (saved == 0xF2)
        goto error;
    if (saved == 0xFF)
        goto error;
    goto done;
receive:
    Wireless_ProcessChildUniExchange(p);
    goto done;
added: {
    u8 *slots = &p->selectedPeer;
    register int incoming asm("r1") = (u8)gUnk_03005390.fields.code;
    *slots |= incoming;
}
    goto done;
removed: {
    u8 *slots = &p->selectedPeer;
    int incoming = (u8)gUnk_03005390.fields.code;
    *slots &= ~incoming;
}
    goto done;
disconnected: {
    u8 *slots = &p->selectedPeer;
    int zero;
    int incoming = (u8)gUnk_03005390.fields.code;
    int selected = *slots & ~incoming;
    zero = 0;
    *slots = selected;
    sub_082238F0(1);
    setOperation(p, 29, zero);
}
    goto done;
reset:
    WirelessController_ResetSession(p);
    goto done;
checkError: {
    union WirelessControllerEvent *event;
    union WirelessControllerEvent *initial = &gUnk_03005390;
    int code = initial->fields.code;
    event = initial;
    if (code != 0x30)
        goto valueCheck;
    if (event->fields.value != 3)
        goto valueCheck;
    event->fields.value = 0;
valueCheck:;
    if (!event->fields.value)
        goto done;
}
error:
    sub_082238F0(1);
    setOperation(p, 28, 0);
done:;
}

void Wireless_OnParentUniSendComplete(void)
{
    struct WirelessController *p = gUnk_02000484;
    u8 slots;
    if (p) {
        u8 *scratch = p->peerRoster;
        int status = rfu_UNI_PARENT_getDRAC_ACK(&slots);
        /* do-while(0) only keeps agbcc's block layout */
        do {
            if (!status) {
                u8 *seen = &p->acknowledgedSlots;
                int bits = (slots << 1) | (*seen);
                int expected;
                *seen = bits;
                expected = scratch[2];
                if ((bits & expected) == expected) {
                    u8 *waiting = &p->awaitingAck;
                    *waiting = status;
                    p->sendReady = status;
                } else {
                    u8 *waiting = &p->awaitingAck;
                    *waiting = 1;
                    p->ackWaitFrames++;
                }
            }
            rfu_REQ_recvData();
        } while (0);
    }
}

void Wireless_OnChildUniClockEvent(u16 v)
{
    if (gUnk_02000484 != 0) {
        if (v == 0x27) {
            rfu_REQ_changeMasterSlave();
            rfu_waitREQComplete();
        } else
            rfu_REQ_recvData();
    }
}

#ifdef NONMATCHING
// fuzzy: 86.7%
// Counter pointer allocation and receive-buffer hoisting still differ.
void Wireless_InitPacketQueue(void *);
void RfuManager_SetEventCallback(void (*)(u8));
void RfuManager_SetMscCallback(void (*)(void));
static inline void beginUniOperation(struct WirelessController *p, int operation, int zero)
{
    p->advanceRequested = zero;
    p->operation = operation;
    p->operationTicks = zero;
    p->operationEntered = 1;
}
void sub_080359E4(struct WirelessController *p)
{
    u8 *roster = p->peerRoster;
    u8 *packet;

    int zero;
    int i;

    u8 *entered;
    u8 *sendPacket;
    u8 *previous, *duplicates, *stale, *missed;
    int bit;
    Bg_ClearTilemap(0);
    {
        int initialZero = 0;
        p->participantIndex = initialZero;
        Wireless_InitPacketQueue(p->packetQueue);
        packet = p->parentUniPacket;
        Memory_Clear(packet, 16);
        Memory_Clear(p->parentUniReceive, 16);
        Wireless_ResetRemoteInputHistory(p);
        p->awaitingAck = initialZero;
        p->slotsChanged = initialZero;
    }
    p->acknowledgedSlots = 1;
    p->receivedSlots = 1;
    p->previousParticipantMask = roster[2];
    roster[2] = (gUnk_03006A80->connSlotFlag << 1) | 1;
    *((u8 *)&p->parentUniPacket + 2) = roster[2];
    packet[0] = 1;
    *((u8 *)&p->parentUniPacket + 1) = 1;
    for (i = 0; i <= 3; i++) {
        if (roster[2] & (2 << i))
            (*((u8 *)&p->parentUniPacket + 1))++;
    }
    p->sendReady = 1;
    i = 0;
    sendPacket = p->parentUniPacket;
    {
        u32 *waitFrames = &p->ackWaitFrames;
        u8 *receiveAhead = &p->receiveAheadCount;
        u8 *sendAhead = &p->sendAheadCount;
        entered = sendPacket - 0x3E;
        previous = p->previousSequence;
        {
            int loopZero = 0;
            duplicates = p->duplicatePackets;
            stale = p->stalePackets;
            missed = entered + 0xAA;
            do {
                {
                    u8 *base = previous;
                    base[i] = loopZero;
                    base = duplicates;
                    base[i] = loopZero;
                }
                stale[i] = loopZero;
                missed[i] = loopZero;
            } while (++i <= 3);
        }
        *waitFrames = 0;
        *receiveAhead = 0;
        *sendAhead = 0;
    }
    RfuManager_SetEventCallback(Wireless_OnParentUniSessionEvent);
    RfuManager_SetMscCallback(Wireless_OnParentUniSendComplete);
    i = 0;
    {
        u8 *receivePacket = p->parentUniReceive;
        do {
            {
                int connected = roster[2];
                bit = 2;
                if (connected & (bit << i))
                    rfu_setRecvBuffer(16, i, receivePacket, 4);
            }
            receivePacket += 4;
        } while (++i <= 3);
    }
    if (!rfu_UNI_setSendData(roster[2] >> 1, sendPacket, 16)) {
        u8 *slot;
        u8 *mode;
        i = 0;
        {
            int slots = roster[2];
            slot = &p->sendSlot;
            mode = &p->sessionMode;
            if (!(slots & bit)) {
                do {
                    i++;
                    if (i > 3)
                        break;
                } while (!(slots & (2 << i)));
            }
        }
        {
            int finalZero = 0;
            *slot = i;
            rfu_UNI_readySendData(*slot);
            *mode = 1;
            p->advanceRequested = finalZero;
            p->operation = 19;
            p->operationTicks = finalZero;
            *entered = 1;
        }
    } else {
        {
            int operation = 27;
            int zero = 0;
            p->advanceRequested = zero;
            p->operation = operation;
            p->operationTicks = zero;
            *entered = 1;
        }
    }
}

#else

INCLUDE_ASM("asm/nonmatching", sub_080359E4);

#endif

#define Wireless_BeginChildUniSession sub_08035B98

void Wireless_BeginChildUniSession(struct WirelessController *p)
{
    u8 *roster = p->peerRoster;
    int index;
    u16 *first;
    u8 *sendPacket, *recvPacket;
    u8 *send, *recv;
    u8 *savedSlots, *parentSlot, *entered;
    u8 *previous, *duplicates, *stale, *missed;
    int zero;
    int status;
    Bg_ClearTilemap(0);
    index = 0;
    {
        int offset = (offsetof(struct WirelessController, peerRoster) + 0x36);
        struct RfuLinkStatus **initial;
        u16 *mine;
        first = (u16 *)((u32)p + offset);
        initial = &gUnk_03006A80;
        mine = &(*(struct RfuLinkStatus *volatile *)initial)->my.id;
        if (*first != *mine) {
            struct RfuLinkStatus **link = &gUnk_03006A80;
            u16 *next = (u16 *)((u32)roster + 0x36);
            do {
                next++;
                index++;
                if (index > 3)
                    break;
            } while (*next != (*(struct RfuLinkStatus *volatile *)link)->my.id);
        }
    }
    p->participantIndex = index;
    Wireless_InitPacketQueue(p->packetQueue);
    send = p->childUniPacket;
    Memory_Clear(send, 4);
    recv = p->childUniPacket + 4;
    Memory_Clear(recv, 16);
    Wireless_ResetRemoteInputHistory(p);
    index = 0;
    sendPacket = send;
    recvPacket = recv;
    savedSlots = &p->previousParticipantMask;
    parentSlot = &p->parentSlot;
    entered = &p->operationEntered;
    previous = p->previousSequence;
    {
        int loopZero = 0;
        duplicates = send + 0x44;
        stale = recv + 0x44;
        missed = p->missedPackets;
        do {
            previous[index] = loopZero;
            duplicates[index] = loopZero;
            stale[index] = loopZero;
            missed[index] = loopZero;
        } while (++index <= 3);
    }
    {
        int slots = roster[2];
        zero = 0;
        *savedSlots = slots;
    }
    *sendPacket = zero;
    RfuManager_SetEventCallback(Wireless_OnChildUniSessionEvent);
    RfuManager_SetMscCallback(Wireless_OnChildUniClockEvent);
    rfu_setRecvBuffer(16, *parentSlot, recvPacket, 16);
    status = rfu_UNI_setSendData((u8)(1 << *parentSlot), sendPacket, 4);
    if (status) {
        {
            int operation = 27;
            p->advanceRequested = zero;
            p->operation = operation;
            p->operationTicks = zero;
            *entered = 1;
        }
    } else {
        p->sessionMode = 1;
        {
            int operation = 20;
            p->advanceRequested = status;
            p->operation = operation;
            p->operationTicks = status;
            *entered = 1;
        }
    }
}

#define Wireless_ClearSlotsIfActive sub_08035CD0

s32 Wireless_ClearSlotsIfActive(struct WirelessController *p)
{
    int result;
    if (!p->active) {
        result = 0;
    } else {
        rfu_clearAllSlot();
        result = 1;
    }
    return result;
}

extern u16 gUnk_03005270;

int Wireless_PublishLocalInput(struct WirelessController *p)
{
    if (p->operationEntered) {
        p->operationEntered = 0;
        ((void (*)(void))Bg_ClearTilemap)();
    }
    {
        int input = gUnk_03005270;
        int mask = 0x2000;
        input |= mask;
        WirelessController_SetLocalInput(p, input);
    }
    p->inputReady = 1;
}

int Wireless_PublishLocalInputAlternate(struct WirelessController *p)
{
    if (p->operationEntered) {
        p->operationEntered = 0;
        ((void (*)(void))Bg_ClearTilemap)();
    }
    {
        int input = gUnk_03005270;
        int mask = 0x2000;
        input |= mask;
        WirelessController_SetLocalInput(p, input);
    }
    p->inputReady = 1;
}

// MATCHING_CONCESSION: register-pinning
void Wireless_OnParentNiSessionEvent(u8 command)
{
    register int key asm("r3") = command;
    int saved = key;
    struct WirelessController *p = gUnk_02000484;
    if (!p)
        goto done;
    if (key == 0x41)
        goto done;
    if (key > 0x41)
        goto high;
    if (key == 0x30)
        goto disconnected;
    if (key > 0x30)
        goto lowMore;
    goto done;
lowMore:
    if (key == 0x32)
        goto removed;
    /* Range switch retains the original strict comparison
                       boundary. */
    switch (key) {
        case 0x32 ... 0x7FFFFFFF:
            break;
        default:
            goto added;
    }
    if (key == 0x33)
        goto disconnected;
    if (key == 0x40)
        goto lost;
    goto done;
high:
    if (key == 0x50)
        goto receive;
    if (key > 0x50)
        goto last;
    if (key == 0x43)
        goto done;
    switch (key) {
        case 0x43 ... 0x7FFFFFFF:
            break;
        default:
            goto reset;
    }
    if (key == 0x44)
        goto stop;
    goto done;
last:
    if (key == 0xF1)
        goto error;
    if (key > 0xF1)
        goto lastMore;
    if (key == 0xF0)
        goto error;
    goto done;
lastMore:
    if (saved == 0xF2)
        goto error;
    if (saved == 0xFF)
        goto error;
    goto done;

added: {
    u8 *slots = &p->selectedPeer;
    register int incoming asm("r1") = (u8)gUnk_03005390.fields.code;
    *slots |= incoming;
}
    goto done;
removed: {
    u8 *slots = &p->selectedPeer;
    int incoming = (u8)gUnk_03005390.fields.code;
    *slots &= ~incoming;
}
    goto done;
disconnected: {
    u8 *slots = &p->selectedPeer;
    union WirelessControllerEvent *event = &gUnk_03005390;
    int zero;
    int remaining;
    int incoming = (u8)event->fields.code;
    int selected = *slots & ~incoming;
    zero = 0;
    *slots = selected;
    p->slotsChanged = 1;
    Wireless_RemoveConnectedSlots(p, (u8)event->fields.code);
    remaining = p->peerRoster[3];
    if (remaining != 1)
        goto done;
    sub_082238F0(1);
    {
        int op = 29;
        p->advanceRequested = zero;
        p->operation = op;
        p->operationTicks = zero;
    }
    p->operationEntered = remaining;
}
    goto done;
lost: {
    int remaining;
    Wireless_RemoveConnectedSlots(p, (u8)gUnk_03005390.fields.code);
    remaining = p->peerRoster[3];
    if (remaining != 1)
        goto done;
    sub_082238F0(1);
    setOperation(p, 29, 0);
    p->operationEntered = remaining;
}
    goto done;
reset:
    WirelessController_ResetSession(p);
    goto done;
stop:
    setOperation(p, 30, 0);
    goto done;
error:
    if (p->operation != 28)
        setOperation(p, 28, 0);
receive:
    gUnk_020000EC = 0;
done:;
}

void Wireless_OnParentNiSendComplete(void)
{
    rfu_REQ_recvData();
}

void Sound_FadeOutChannel10(int);

void Wireless_BeginParentNiSession(struct WirelessController *p)
{
    u8 *scratch = p->peerRoster;
    int i;
    int bit;
    u32 listener;
    Bg_ClearTilemap(0);
    Wireless_InitPacketQueue(p->packetQueue);
    Memory_Clear(p->niSend, 12);
    Memory_Clear(p->niReceive, 12);
    bit = 8;
    rfu_clearSlot(bit, p->sendSlot);
    RfuManager_SetEventCallback(Wireless_OnParentNiSessionEvent);
    RfuManager_SetMscCallback(Wireless_OnParentNiSendComplete);
    i = 0;
    /* do-while(0) only keeps agbcc's block layout */
    do {
        int mask;
        int active = scratch[2];
        if (scratch) {
            bit = 2;
            mask = bit << i;
        } else {
            bit = 2;
            mask = bit << i;
        }
        if (active & mask) {
            rfu_setRecvBuffer(32, i, p->niReceive, 12);
        }
    } while ((++i) <= 3);
    p->sessionMode = bit;
    setOperation(p, 23, 0);
    listener = p->sessionStartScript;
    if (listener) {
        Script_Call(listener, 0);
    }
    Sound_FadeOutChannel10(4);
}

// MATCHING_CONCESSION: register-pinning
int Wireless_TryStartParentNiSend(struct WirelessController *p)
{
    int flags = p->handshakeFlags;
    struct RfuLinkStatus **links;
    int one;
    u8 *slot;
    int index;
    if (flags & 2)
        goto failed;
    links = &gUnk_03006A80;
    if ((*links)->linkLossSlotFlag)
        goto failed;
    one = 1;
    {
        int check = one;
        check &= flags;
        asm("" : "+r"(check));
        if (!check)
            goto ready;
    }
    slot = &p->sendSlot;
    rfu_clearSlot(4, *slot);
    if (rfu_NI_setSendData((u8)(one << *slot), 15, (p->niSend), 12))
        goto sendError;
    index = 0;
    if (!((*links)->connSlotFlag & one)) {
        struct RfuLinkStatus **cursor = links;
        int bit = 1;
        do {
            index++;
            if (index > 3)
                break;
        } while (!(((*(struct RfuLinkStatus *volatile *)cursor)->connSlotFlag >> index) & bit));
    }
    *slot = index;
    p->handshakeFlags = (p->handshakeFlags & 0xFFFE) | 2;
    goto ready;
sendError:
    Wireless_SetStatus(2);
failed:
    return 0;
ready:
    return 1;
}

void sub_08249238(void *);

// MATCHING_CONCESSION: register-pinning
void Wireless_PollParentNiSend(struct WirelessController *p)
{
    register int lost asm("r6") = 0;
    if (p->handshakeFlags & 2) {
        struct RfuSlotStatusNI **slots = gUnk_03006A70;
        u8 *slot = &p->sendSlot;
        int state = slots[*slot]->send.state;
        if (state == 0x26) {
            void *callback = p->onSend;
            if (callback)
                Callback_InvokeActor((p->niSend), callback);
            {
                struct NIComm *send = &slots[*slot]->send;
                u8 *original = &send->bmSlotOrg;
                int old = *original;
                {
                    register int current asm("r0") = send->bmSlot;
                    asm("" : "+r"(current));
                    lost = current;
                    lost ^= old;
                }
            }
            rfu_clearSlot(4, *slot);
            p->handshakeFlags &= 0xFFFD;
        } else if (state == 0x27) {
            void *callback = *(void **)(((u8 *)&p->onSendFailure));
            if (callback)
                sub_08249238(callback);
            {
                struct NIComm *send = &slots[*slot]->send;
                u8 *original = &send->bmSlotOrg;
                int old = *original;
                {
                    register int current asm("r0") = send->bmSlot;
                    asm("" : "+r"(current));
                    lost = current;
                    lost ^= old;
                }
            }
            rfu_clearSlot(4, *slot);
            p->handshakeFlags = 0;
        }
        if (lost) {
            lost |= p->lostSlots;
            p->lostSlots = lost;
        }
    }
}

int Wireless_EnterParentNiTransferWait(struct WirelessController *p)
{
    u8 *pending = &p->operationEntered;
    if (*pending)
        *pending = 0;
    setOperation(p, 23, 0);
    *pending = 1;
    return 0;
}

int Wireless_PollParentNiReceive(struct WirelessController *p)
{
    u8 *slot;
    int state;
    if (p->operationEntered)
        p->operationEntered = 0;
    {
        struct RfuSlotStatusNI **slots = gUnk_03006A70;
        slot = &p->sendSlot;
        state = slots[*slot]->recv.state;
    }
    if (state == 0x46) {
        void *callback = p->onReceive;
        if (callback)
            Callback_InvokeActor((p->niReceive), callback);
        rfu_clearSlot(8, *slot);
    } else if ((u16)(state - 0x47) <= 1) {
        void *callback = *(void **)(((u8 *)&p->onReceiveFailure));
        if (callback)
            sub_08249238(callback);
    }
    return 0;
}

u32 Wireless_ClearParentTransferEntryFlag(struct WirelessController *p)
{
    u8 *q = &p->operationEntered;
    if (*q)
        *q = 0;
    return 0;
}

static inline void setChildTransferOperation(struct WirelessController *p, int op, int zero)
{
    p->advanceRequested = zero;
    p->operation = op;
    p->operationTicks = zero;
    {
        u8 *pending = &p->operationEntered;
        *pending = 1;
    }
}

// MATCHING_CONCESSION: register-pinning
void Wireless_OnChildTransferEvent(u8 command)
{
    register int key asm("r3") = command;
    int saved = key;
    struct WirelessController *p = gUnk_02000484;
    if (!p)
        goto done;
    if (key > 0x41)
        goto high;
    /* Range switch retains the original strict comparison
                       boundary. */
    switch (key) {
        case 0x40 ... 0x7FFFFFFF:
            goto done;
        default:
            break;
    }
    if (key == 0x30)
        goto disconnected;
    if (key <= 0x30)
        goto done;
    if (key == 0x32)
        goto removed;
    switch (key) {
        case 0x32 ... 0x7FFFFFFF:
            break;
        default:
            goto added;
    }
    if (key == 0x33)
        goto disconnected;
    goto done;
high:
    if (key == 0x50)
        goto receive;
    if (key > 0x50)
        goto last;
    if (key == 0x42)
        goto reset;
    goto done;
last:
    if (key == 0xF1)
        goto error;
    if (key > 0xF1)
        goto lastMore;
    if (key == 0xF0)
        goto error;
    goto done;
lastMore:
    if (saved == 0xF2)
        goto error;
    if (saved == 0xFF)
        goto error;
    goto done;
receive:
    RfuManager_SendData(1);
    goto done;
added: {
    u8 *slots = &p->selectedPeer;
    register int incoming asm("r1") = (u8)gUnk_03005390.fields.code;
    *slots |= incoming;
}
    goto done;
removed: {
    u8 *slots = &p->selectedPeer;
    int incoming = (u8)gUnk_03005390.fields.code;
    *slots &= ~incoming;
}
    goto done;
disconnected: {
    u8 *slots = &p->selectedPeer;
    int zero;
    int incoming = (u8)gUnk_03005390.fields.code;
    int selected = *slots & ~incoming;
    zero = 0;
    *slots = selected;
    setChildTransferOperation(p, 29, zero);
}
    goto done;
reset:
    WirelessController_ResetSession(p);
    goto done;
error:
    if (p->operation != 28)
        setChildTransferOperation(p, 28, 0);
done:;
}

void Wireless_OnChildTransferReceiveRequest(void)
{
    if (gUnk_02000484)
        rfu_REQ_recvData();
}

// MATCHING_CONCESSION: register-pinning
void Wireless_BeginChildTransfer(struct WirelessController *p)
{
    int index;
    u16 *ids;
    struct RfuLinkStatus **links;
    u8 *buffer;
    u32 listener;
    int zero;
    Bg_ClearTilemap(0);
    index = 0;
    {
        register int offset asm("r0") = 0x16A;
        u16 *first;
        struct RfuLinkStatus **initial;
        u16 *mine;
        first = (u16 *)((u8 *)p + offset);
        initial = &gUnk_03006A80;
        mine = &(*initial)->my.id;
        if (*first != *mine) {
            struct RfuLinkStatus **cursor = initial;
            int offset = (offsetof(struct WirelessController, peerRoster) + 0x36);
            u16 *next;
            next = (u16 *)((u8 *)p + offset);
            do {
                next++;
                index++;
                if (index > 3)
                    break;
            } while (*next != (*(struct RfuLinkStatus *volatile *)cursor)->my.id);
        }
    }
    zero = 0;
    p->participantIndex = index;
    Wireless_InitPacketQueue(p->packetQueue);
    Memory_Clear((p->niSend), 12);
    buffer = (p->niReceive);
    Memory_Clear(buffer, 12);
    RfuManager_SetEventCallback(Wireless_OnChildTransferEvent);
    RfuManager_SetMscCallback(Wireless_OnChildTransferReceiveRequest);
    rfu_setRecvBuffer(32, p->parentSlot, buffer, 12);
    p->sessionMode = 2;
    setOperation(p, 24, zero);
    listener = p->sessionStartScript;
    if (listener)
        Script_Call(listener, 0);
}

// MATCHING_CONCESSION: register-pinning
int Wireless_ExchangeChildNiPacket(struct WirelessController *argument)
{
    struct WirelessController *p = argument;
    int flags = p->handshakeFlags;
    struct RfuLinkStatus **links;
    int one;
    u8 *slot;
    int index;
    {
        register int test asm("r0") = 2 & flags;
        if (test)
            goto failed;
    }
    if (gUnk_03006A80->linkLossSlotFlag)
        goto failed;
    {
        struct RfuSlotStatusNI **slots = gUnk_03006A70;
        register u8 *initial asm("r2");
        int state;
        register int index asm("r0");
        initial = &p->parentSlot;
        index = *initial;
        asm("" : "+r"(index));
        state = slots[index]->recv.state;
        slot = initial;
        if (state == 0x46) {
            void *callback = p->onReceive;
            if (callback)
                Callback_InvokeActor((p->niReceive), callback);
            rfu_clearSlot(8, *slot);
        } else if ((u16)(state - 0x47) <= 1) {
            void *callback = *(void **)(((u8 *)&p->onReceiveFailure));
            if (callback)
                sub_08249238(callback);
        }
    }
    flags = p->handshakeFlags;
    one = 1;
    {
        int check = one;
        asm("" : "+r"(check));
        check &= flags;
        if (!check)
            goto ready;
    }
    rfu_clearSlot(4, *slot);
    if (rfu_NI_setSendData((u8)(one << *slot), 14, (p->niSend), 12))
        goto sendError;
    index = 0;
    links = &gUnk_03006A80;
    if (!((*links)->connSlotFlag & one)) {
        struct RfuLinkStatus **cursor = links;
        int bit = 1;
        do {
            index++;
            if (index > 3)
                break;
        } while (!(((*(struct RfuLinkStatus *volatile *)cursor)->connSlotFlag >> index) & bit));
    }
    *slot = index;
    p->handshakeFlags = (p->handshakeFlags & 0xFFFE) | 2;
    goto ready;
sendError:
    Wireless_SetStatus(2);
failed:
    return 0;
ready:
    return 1;
}

// MATCHING_CONCESSION: register-pinning
void Wireless_PollChildNiSend(struct WirelessController *p)
{
    int lost = 0;
    if (p->handshakeFlags & 2) {
        struct RfuSlotStatusNI **slots = gUnk_03006A70;
        u8 *slot = &p->parentSlot;
        int state = slots[*slot]->send.state;
        if (state == 0x26) {
            void *callback = p->onSend;
            if (callback)
                Callback_InvokeActor((p->niSend), callback);
            rfu_clearSlot(4, *slot);
            p->handshakeFlags &= 0xFFFD;
        } else if (state == 0x27) {
            void *callback = *(void **)(((u8 *)&p->onSendFailure));
            if (callback)
                sub_08249238(callback);
            {
                struct NIComm *send = &slots[*slot]->send;
                u8 *original = &send->bmSlotOrg;
                register int old asm("r3") = *original;
                {
                    register int current asm("r0") = send->bmSlot;
                    asm("" : "+r"(current));
                    lost = current;
                    lost ^= old;
                }
            }
            rfu_clearSlot(4, *slot);
            p->handshakeFlags = 0;
        }
        if (lost) {
            lost |= p->lostSlots;
            asm("" : "+r"(lost));
            p->lostSlots = lost;
        }
    }
}

int Wireless_EnterChildTransferWait(struct WirelessController *p)
{
    u8 *pending = &p->operationEntered;
    if (*pending)
        *pending = 0;
    setOperation(p, 23, 0);
    *pending = 1;
    return 0;
}

u32 Wireless_ClearChildTransferEntryFlag(struct WirelessController *p)
{
    u8 *q = &p->operationEntered;
    if (*q)
        *q = 0;
    return 0;
}

u32 Wireless_ClearChildTransferWaitEntryFlag(struct WirelessController *p)
{
    u8 *q = &p->operationEntered;
    if (*q)
        *q = 0;
    return 0;
}

void sub_08223BC0(int);

extern void *gUnk_086056EC[];

int Wireless_UpdateConnectionSession(struct WirelessController *argument)
{
    struct WirelessController *p = argument;
    if (p->adapterInitialized) {
        volatile u8 *busy = &gUnk_020000EC;
        int initial = *busy;
        u8 *lost = &p->lostSlots;
        if (initial == 1) {
            do {
            } while (*busy == 1);
        }
        sub_08223BC0(0);
        if (*lost) {
            rfu_REQ_disconnect(*lost);
            rfu_waitREQComplete();
            *lost = 0;
        }
        {
            int mode = gUnk_03006A80->parentChild;
            if (mode == 1) {
                gUnk_020000EC = mode;
                rfu_REQ_sendData(1);
            }
        }
    }
    WirelessController_UpdatePeerRosterSend(p);
    WirelessController_PollPeerRosterSend(p);
    Callback_InvokeActor(p, gUnk_086056EC[p->operation]);
}

int Wireless_UpdateInactiveConnectionSession(struct WirelessController *p)
{
    if (p->adapterInitialized) {
        p->availableSlots = 0;
        sub_08223BC0(0);
    }
    Callback_InvokeActor(p, gUnk_086056EC[p->operation]);
}

extern u32 gUnk_03005250;

extern u16 gUnk_03005274[];

static inline void publishInputs(struct WirelessController *p)
{
    int enabled = p->inputReady;
    if (enabled) {
        register int i asm("r5");
        int valid;
        int mask;
        u16 *output;
        int invalid;
        register u16 *history asm("r4");
        gUnk_03005250 = 1;
        i = 0;
        {
            int initial = 0x2000;
            valid = initial;
        }
        mask = 4095;
        output = gUnk_03005274;
        {
            register int initial asm("r1") = 65535;
            asm("" : "+r"(initial));
            invalid = initial;
        }
        history = p->inputHistory;
        do {
            register int value asm("r2") = *history;
            int test = valid;
            int result;
            if (value & test) {
                result = mask;
                result &= value;
            } else
                result = *output | invalid;
            *output = result;
            output++;
            history++;
        } while (++i <= 4);
    } else
        gUnk_03005250 = enabled;
}

// MATCHING_CONCESSION: register-pinning
int Wireless_UpdateParentUniSession(struct WirelessController *p)
{
    p->inputReady = 0;
    while (*(volatile u8 *)&gUnk_020000EC == 1)
        ;
    sub_08223BC0(0);
    {
        /* Byte-base adaptation keeps the original pointer/store allocation. */
        u8 *changed = (u8 *)p + 0xE9;
        if (*changed) {
            u8 *waiting = (u8 *)p + 0xE8;
            if (*waiting) {
                *waiting = 0;
                ((u8 *)p)[0xE7] = 1;
                ((u8 *)p)[0xE6] = 1;
                *changed = 0;
            }
        }
    }
    WirelessController_RestoreLocalInput(p);
    Wireless_BuildParentUniPacket(p);
    if (!gUnk_03006A80->linkLossSlotFlag && p->sendReady) {
        gUnk_020000EC = 1;
        if (!p->awaitingAck) {
            rfu_UNI_changeAndReadySendData(p->sendSlot, (p->parentUniPacket), 16);
            RfuManager_SendData(1);
        } else
            rfu_REQ_PARENT_resumeRetransmitAndChange();
    }
    WirelessController_PollInputExchange(p);
    Callback_InvokeActor(p, gUnk_086056EC[p->operation]);
    publishInputs(p);
}

// MATCHING_CONCESSION: register-pinning
int Wireless_UpdateChildUniSession(struct WirelessController *p)
{
    u8 *ready = &p->inputReady;
    {
        int zero = 0;
        *ready = zero;
        sub_08223BC0(zero);
    }
    rfu_getMasterSlave();
    WirelessController_RestoreLocalInput(p);
    WirelessController_PollInputExchange(p);
    {
        void **handlers = gUnk_086056EC;
        int op = p->operation;
        Callback_InvokeActor(p, handlers[op]);
    }
    publishInputs(p);
}

void Wireless_UpdateParentNiSession(struct WirelessController *argument)
{
    struct WirelessController *p = argument;
    volatile u8 *busy = &gUnk_020000EC;
    u8 *lost;
    while (*busy == 1)
        ;
    sub_08223BC0(0);
    lost = &p->lostSlots;
    if (*lost) {
        rfu_REQ_disconnect(*lost);
        rfu_waitREQComplete();
        *lost = 0;
    }
    {
        int mode = gUnk_03006A80->parentChild;
        if (mode == 1) {
            gUnk_020000EC = mode;
            RfuManager_SendData(1);
        }
    }
    Wireless_TryStartParentNiSend(p);
    Wireless_PollParentNiSend(p);
    Callback_InvokeActor(p, gUnk_086056EC[p->operation]);
}

int Wireless_UpdateChildNiSession(struct WirelessController *p)
{
    u8 *ready = &p->inputReady;
    {
        int zero = 0;
        *ready = zero;
        sub_08223BC0(zero);
    }
    rfu_getMasterSlave();
    Wireless_ExchangeChildNiPacket(p);
    Wireless_PollChildNiSend(p);
    {
        void **handlers = gUnk_086056EC;
        int op = p->operation;
        Callback_InvokeActor(p, handlers[op]);
    }
}

extern u32 gUnk_020000F0, gUnk_0300546C, gUnk_03005408;

void Solar_PauseMeasurement(void);

void Solar_ResumeMeasurement(void);

void Task_UnlinkFromPriorityList(void *);

int Wireless_UpdateControllerTask(struct WirelessController *p)
{
    Solar_PauseMeasurement();
    gUnk_020000F0 = p->status;
    switch (p->sessionMode) {
        case 0:
            if (p->active)
                Wireless_UpdateConnectionSession(p);
            else
                Wireless_UpdateInactiveConnectionSession(p);
            break;
        case 1:
            if (p->active)
                Wireless_UpdateParentUniSession(p);
            else
                Wireless_UpdateChildUniSession(p);
            break;
        case 2:
            if (p->active)
                Wireless_UpdateParentNiSession(p);
            else
                Wireless_UpdateChildNiSession(p);
            break;
    }
    if (p->resetRequested) {
        int zero = 0;
        p->resetRequested = zero;
        sub_082238F0(1);
        setOperation(p, zero, zero);
    }
    if (p->destroyRequested) {
        gUnk_0300546C = 0;
        gUnk_02000484 = 0;
        Task_UnlinkFromPriorityList(p);
        Heap_Free(p);
        Solar_ResumeMeasurement();
    } else {
        if ((u16)(p->operation - 9) <= 17)
            gUnk_03005408 = 1;
        p->operationTicks++;
        p->frameCounter++;
    }
    return 0;
}

extern u32 gUnk_020000F0;

s32 Wireless_InitRfuApi(void);

u32 Script_GetTextSourcePc(void);

s32 Wireless_InitControllerFromScript(struct WirelessController *p)
{
    u32 z;
    u8 *f = &p->status;
    z = 0;
    *f = z;
    gUnk_020000F0 = z;
    if (Wireless_InitRfuApi() < 0)
        goto fail;
    p->destroyRequested = z;
    WirelessController_ResetSession(p);
    {
        u32 one = 1;
        p->advanceRequested = z;
        p->operation = one;
        p->operationTicks = z;
        p->operationEntered = 1;
    }
    p->sessionStartScript = Script_GetKeywordValue(0x70, 0);
    if (!Script_SeekToKeyword(0x61))
        goto fail;
    {
        u32 v = Script_GetTextSourcePc();
        p->identityScript = (void *)v;
        if (v == 0) {
        fail:
            return -1;
        }
    }
    return 0;
}

int SunClock_GetSecond(void);

void Task_LinkToPriorityList(void *);

extern u8 gUnk_03003A50;

extern u32 gUnk_0300546C;

// MATCHING_CONCESSION: register-pinning
struct WirelessController *Wireless_CreateController(void)
{
    struct WirelessController **singleton = &gUnk_02000484;
    struct WirelessController *existing = *singleton;
    struct WirelessController *p;
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
        u32 *position = &gUnk_03005308;
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
        table = gUnk_0203B400;
        term = *(const u16 *)(index + (u32)table);
        saltAddress = &gUnk_03003A50;
        byteMask = 255;
        salt = *saltAddress;
        random += term;
        random &= byteMask;
        random <<= 8;
        salt |= random;
        destination = (u16 *)(((u8 *)&p->gameId));
        *destination = salt;
    }
    if (Wireless_InitControllerFromScript(p) >= 0) {
        int one;
        Task_SetCallbacks(p, 0, 0);
        one = 1;
        p->task.list = one;
        /* Engine Task has no named byte for this observed scheduler flag. */
        ((u8 *)&p->task)[0x16] = one;
        p->task.id = (u32)existing;
        Task_LinkToPriorityList(p);
        gUnk_0300546C = one;
        *singleton = p;
        return p;
    }
    Heap_Free(p);
failed:
    return 0;
}

void Wireless_UpdateController(void)
{
    if (gUnk_02000484)
        Wireless_UpdateControllerTask(gUnk_02000484);
}

void Wireless_RequestInputResync(void)
{
    struct WirelessController *p = gUnk_02000484;
    if (p)
        p->resyncRequested = 1;
}

/* Wireless_GetLocalParticipantIndex: Returns the participant index assigned by matching the local RFU ID */

#define Wireless_GetLocalParticipantIndex sub_080369F0

s32 sub_080369F0(void)
{
    if (gUnk_02000484 == NULL)
        return -1;
    return (s8)gUnk_02000484->participantIndex;
}

int Wireless_GetConnectedParentSlot(void)
{
    struct WirelessController *p = gUnk_02000484;
    if (!p)
        goto absent;
    if (!gUnk_03006A80->parentChild)
        goto read;
absent:
    return 0;
read:
    return p->parentSlot;
}

int Wireless_IsSessionConnected(void)
{
    struct WirelessController *p = gUnk_02000484;
    if (!p)
        goto absent;
    if (p->active) {
        if (p->operation <= 4)
            goto absent;
        if (gUnk_03006A80->parentChild == 255)
            goto absent;
        goto present;
    }
    if (gUnk_03006A80->parentChild == 0)
        goto present;
absent:
    return 0;
present:
    return 1;
}

u8 *Wireless_GetPeerHandshakeData(void)
{
    struct WirelessController *p = gUnk_02000484;
    if (!p)
        return 0;
    return &p->peerRoster[0];
}

u32 Wireless_IsSessionActive(void)
{
    return gUnk_02000484 ? gUnk_02000484->active : 0x0;
}

u8 Wireless_GetConnectedSlotMask(void)
{
    if (gUnk_02000484 == 0)
        return 0;
    return gUnk_02000484->connectedSlots;
}

u16 Wireless_GetOperation(void)
{
    if (gUnk_02000484 == 0)
        return 0;
    return gUnk_02000484->operation;
}

void Wireless_RequestDisconnectPeer(void)
{
    struct WirelessController *p = gUnk_02000484;
    if (p)
        p->peerRefreshRequested = 1;
}

void Wireless_RequestAdvanceConnection(void)
{
    struct WirelessController *p = gUnk_02000484;
    if (p)
        p->advanceRequested = 1;
}

void Wireless_SetTransferCallbacks(void *sent, void *received, void *sendFailure,
                                   void *receiveFailure)
{
    struct WirelessController *p = gUnk_02000484;
    if (p) {
        p->onSend = sent;
        p->onReceive = received;
        *(void **)(((u8 *)&p->onSendFailure)) = sendFailure;
        *(void **)(((u8 *)&p->onReceiveFailure)) = receiveFailure;
    }
}

int Wireless_QueueNiPacket(u8 *packet)
{
    struct WirelessController *p = gUnk_02000484;
    if (!p)
        goto busy;
    if (p->handshakeFlags & 2)
        goto busy;
    {
        u8 *destination = (p->niSend);
        (*packet)++;
        CpuSet(packet, destination, 0x04000003);
    }
    p->handshakeFlags |= 1;
    return 0;
busy:
    return -1;
}

/* Wireless_CopyReceivedNiPacket: Copies the twelve-byte NI receive buffer into the caller destination */

#define Wireless_CopyReceivedNiPacket sub_08036BAC

s32 sub_08036BAC(void *dst)
{
    if (gUnk_02000484 == NULL)
        return -1;
    CpuSet(gUnk_02000484->niReceive, dst, 0x04000003);
    return 0;
}

void Wireless_SetStatus(u32 v)
{
    if (gUnk_02000484 != 0) {
        gUnk_02000484->status = v;
    }
}

u32 Wireless_GetStatus(void)
{
    struct WirelessController *p = gUnk_02000484;
    u32 r;
    if (p != 0)
        r = p->status;
    else
        r = gUnk_020000F0;
    return r;
}

void Wireless_RequestDisconnectMode(void)
{
    struct WirelessController *p = gUnk_02000484;
    if (p) {
        p->disconnectMode = 1;
        setOperation(p, 31, 0);
    }
}

void Wireless_RequestResetConnection(void)
{
    if (gUnk_02000484) {
        gUnk_02000484->resetRequested = 1;
    }
}

void Wireless_RequestDestroyController(void)
{
    struct WirelessController *p = gUnk_02000484;
    if (p)
        p->destroyRequested = 1;
}

/* Wireless_GetLocalIdentityData: Tentatively returns the local identity storage beginning at controller offset 0x50 */

#define Wireless_GetLocalIdentityData sub_08036C6C

u8 *sub_08036C6C(void)
{
    if (gUnk_02000484 == NULL)
        return NULL;
    return gUnk_02000484->gameName;
}

u16 Wireless_GetLocalSessionId(void)
{
    u16 r;
    if (gUnk_02000484)
        r = gUnk_02000484->gameId;
    else
        r = 0;
    return r;
}
