#include "wireless_game_items_and_display.h"

void WirelessGame_AddItem(void *p, int index)
{
    struct WirelessGameExchange *q = WirelessExchange_Get();
    if (q) {
        u8 *counts = q->counts;
        if (counts[index] <= 4) {
            int total = 0, i = 0;
            do {
                total += counts[i];
            } while (++i <= 3);
            if (total <= 9) {
                WirelessGameItems_QueueLocalItem(p, index, 1);
                counts[index]++;
                WirelessGame_AddWeightedScore(2, 1);
                Sound_PlaySong(885);
            }
        }
    }
}

int WirelessGame_TryAddCyclingItem(void *owner)
{
    struct WirelessGameExchange *p = WirelessExchange_Get();
    int index;
    u8 *choice, *counts;
    if (!p)
        goto unavailable;
    {
        if (!++p->counter) {
            int mask = 255;
            p->counter |= mask;
        }
        if (p->counter != 1) {
        unavailable:
            return 0;
        }
        {
            {
                u8 *field = &p->choice;
                int value;
                value = *field;
                choice = field;
                switch (value) {
                    case 8:
                        index = 3;
                        break;
                    case 7:
                        index = 1;
                        break;
                    case 6:
                    default:
                        index = 2;
                        break;
                }
            }
            counts = p->counts;
            if (counts[index] <= 4) {
                int total = 0, i = 0;
                do {
                    total += counts[i];
                } while (++i <= 3);
                if (total <= 9) {
                    WirelessGameItems_QueueLocalItem(owner, index, 1);
                    counts[index]++;
                    WirelessGame_AddWeightedScore(2, 1);
                    Sound_PlaySong(885);
                    if (++*choice > 8)
                        *choice = 6;
                }
            }
            return 1;
        }
    }
}

void WirelessGame_NotifyPlayerDefeated(void)
{
    struct WirelessGameExchange *p = WirelessExchange_Get();
    if (p)
        p->playerDefeated = 1;
}

void WirelessGame_NotifyOpponentDefeated(void)
{
    struct WirelessGameExchange *p = WirelessExchange_Get();
    if (p)
        p->opponentDefeated = 1;
}

/* WirelessGame_ConsumePendingEvent: Returns and clears the pending event flag at offset 0x30 */

#define WirelessGame_ConsumePendingEvent sub_08039708

u32 sub_08039708(void)
{
    struct WirelessGameExchange *p = WirelessExchange_Get();
    u8 *flag;
    if (p == NULL || *(flag = &p->pendingEvent) == 0)
        return 0;
    *flag = 0;
    return 1;
}

static inline u8 GetPlayerHealth(struct WirelessGameRemoteStats *q)
{
    return q->playerHealthPercent;
}

u32 WirelessGame_GetRemotePlayerHealthPercent(void)
{
    struct WirelessGameExchange *p = WirelessExchange_Get();
    u32 r;
    if (p)
        r = GetPlayerHealth(&p->remote);
    else
        r = 0;
    return r;
}

static inline u8 GetOpponentHealth(struct WirelessGameRemoteStats *q)
{
    return q->opponentHealthPercent;
}

u32 WirelessGame_GetRemoteOpponentHealthPercent(void)
{
    struct WirelessGameExchange *p = WirelessExchange_Get();
    u32 r;
    if (p)
        r = GetOpponentHealth(&p->remote);
    else
        r = 0;
    return r;
}

u32 WirelessGame_IsItemToggleEnabled(void)
{
    struct WirelessGameExchange *p = WirelessExchange_Get();
    if (!p)
        return 0;
    return p->enabled;
}

// MATCHING_CONCESSION: register-pinning
int WirelessGame_ToggleItemMode(void)
{
    struct WirelessGameExchange *p = WirelessExchange_Get();
    asm("" : "+r"(p));
    if (p && p->enabled) {
        /* Address the controller representation: byte 6 is itemMode, beyond
         * the four-element counts array. Keep this base for matching. */
        u8 *v = (u8 *)p + (u32) & ((struct WirelessGameExchange *)0)->counts;
        v[6] = 1 - v[6];
        Sound_PlaySong(267);
        return v[6];
    }
    return 0;
}

u32 WirelessGame_GetRoundCount(void)
{
    struct WirelessGameExchange *p = WirelessExchange_Get();
    if (!p)
        return 0;
    return p->roundCount;
}

/* WirelessGame_SuppressPlayerReset: Sets global flag 0x1000 which suppresses a player reset call; interpretation tentative */

#define WirelessGame_SuppressPlayerReset sub_080397C0

void sub_080397C0(void)
{
    gEngineFlags |= 0x1000;
}

void WirelessGame_AddWeightedScore(int index, int amount)
{
    struct WirelessGameExchange *p = WirelessExchange_Get();
    if (p) {
        int *total = &p->total;
        int weight = p->weights[index];
        {
            int value = weight;
            value *= amount;
            *total += value;
        }
    }
}

/* WirelessGame_IsRoundActive: Tests whether the controller is in gameplay state 7 */

#define WirelessGame_IsRoundActive sub_08039800

u32 sub_08039800(void)
{
    struct WirelessGameExchange *p = WirelessExchange_Get();
    if (p == NULL || p->state != 7)
        return 0;
    return 1;
}

void WirelessGame_ResetCyclingItemLatch(void)
{
    struct WirelessGameExchange *p = WirelessExchange_Get();
    if (p)
        p->counter = 0;
}

void WirelessExchange_OnPacketSent(void);

void WirelessExchange_OnPacketReceived(void);

void WirelessExchange_OnSendFailure(void);

void WirelessExchange_OnReceiveFailure(void);

void Wireless_SetTransferCallbacks(void *, void *, void *, void *);

void WirelessLinkIndicator_SetPosition(int, int);

void WirelessGame_InitDisplayAndLinkCallbacks(struct WirelessGameExchange *p)
{
    WirelessGameItems_Create(0, 0);
    p->display = WirelessGameDisplay_Create();
    Wireless_SetTransferCallbacks(WirelessExchange_OnPacketSent, WirelessExchange_OnPacketReceived,
                                  WirelessExchange_OnSendFailure,
                                  WirelessExchange_OnReceiveFailure);
    WirelessLinkIndicator_SetPosition(4, 24);
}

void WirelessGame_RestartSession(void)
{
    struct WirelessGameExchange *p = WirelessExchange_Get();
    u8 *alias2;
    struct WirelessGameExchange *alias;
    if (p) {
        int one = 1;
        u8 *state = alias2 = &p->state;
        int zero = 0;
        *state = one;
        state++;
        *state = one;
        p->timer = zero;
        alias = p;
        WirelessGame_InitDisplayAndLinkCallbacks(alias);
    }
}

void WirelessGame_BeginResults(void)
{
    struct WirelessGameExchange *p = WirelessExchange_Get();
    if (p) {
        int state = 12;
        u8 *dest = &p->state;
        int zero = 0;
        *dest = state;
        p->entered = 1;
        p->timer = zero;
    }
}

u32 WirelessGame_GetSelectedEquipmentEffect(void)
{
    struct GameState *g = gGameState;
    u32 r;
    /* Add the selected offset before the table base: direct slots88 indexing
     * changes agbcc address/register ordering. */
    switch (
        (s16)(*(u16 *)((u8 *)g + g->selected * 2 + (u32) & ((struct GameState *)0)->slots88) - 6)) {
        case 0:
            r = 1;
            break;
        case 1:
            r = 4;
            break;
        case 2:
            r = 6;
            break;
        case 3:
        case 4:
        case 5:
            r = 3;
            break;
        default:
            r = 0;
            break;
    }
    return r;
}

void WirelessGameItem_SetRandomArcVelocity(struct WirelessGameItem *arg, int baseArg, int mask)
{
    struct WirelessGameItem *p = arg;
    int base = baseArg;
    u16 *table = gUnk_0203B400;
    u32 *state = &gRandomIndex;
    u32 limit;
    u32 index;
    int random;
    index = (*state) + 1;
    limit = 1023;
    index &= limit;
    random = index * 2;
    random = *((u16 *)(random + ((u32)table)));
    random &= mask;
    base += random;
    index = index + 1;
    index = index & limit;
    *state = index;
    {
        int sign = *((u8 *)(&table[index]));
        if (sign <= 127) {
            p->arcVelocity = base;
        } else {
            int negative = -base;
            p->arcVelocity = negative;
        }
    }
}

void WirelessGameItem_SetRandomNegativeArcVelocity(struct WirelessGameItem *p, int base, int mask)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    {
        u32 offset = index * 2;
        int random;
        random = *(u16 *)((u8 *)gUnk_0203B400 + offset);
        random &= mask;
        {
            int result = base + random;
            p->arcVelocity = -result;
        }
    }
}

void WirelessGameItems_ApplyMotionWave(struct WirelessGamePosition *arg, int angleArg, int scale)
{
    struct WirelessGamePosition *p = arg;
    const s16 *table;
    int angle = angleArg;
    int product;
    int delta;
    angle *= 4;
    table = gUnk_086149C4;
    angle &= 255;
    product = table[angle] * scale;
    if (product >= 0)
        delta = product >> 12;
    else
        delta = -((-product) >> 12);
    p->x += delta;
    p->y += delta;
}

void WirelessGameItems_UpdateHiddenItem(struct WirelessGameItems *p, struct WirelessGameItem *q)
{
    if (q->entered) {
        q->entered = 0;
        q->sprite.flags |= 1;
        q->drawing.flags |= 1;
    }
}

extern const u16 gUnk_0824DE7C[];

// MATCHING_CONCESSION: register-pinning
extern const u16 gUnk_0824DE8C[];

static inline void Item_LoadFrame(struct WirelessGameItems *owner, struct WirelessGameItem *p,
                                  int index)
{
    int choice;
    int count = p->count;
    struct WirelessGameSprite *sprite = &p->sprite;
    struct WirelessGameDrawing *drawing;
    sprite->value = count;
    p->sprite.flags &= ~1;
    {
        struct WirelessGameDrawing *d = drawing = &p->drawing;
        struct WirelessGameResource *res = &owner->resource;
        if (index > 7)
            choice = (index & 3) + 4;
        else
            choice = index & 3;
        Anim_LoadKeyframe(d, res, gUnk_0824DE7C[choice], 0);
    }
    p->drawing.flags &= ~1;
}

static inline u32 Item_NextRandom(void)
{
    register const u16 *t asm("r6") = gUnk_0203B400;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return t[gRandomIndex];
}

static inline void Item_Place(struct WirelessGameItem *p, struct WirelessGameSprite *sprite,
                              struct WirelessGameDrawing *drawing, int index)
{
    struct WirelessGamePosition *pos = &p->drawing.position.coords;
    int y = 0x80;
    if (index > 7)
        y = 0x90;
    pos->x = gUnk_0824DE8C[index & 3] + 0x50;
    pos->y = y;
    pos->z = 0;
    if (p->shakeTicks) {
        u32 r = Item_NextRandom();
        {
            s32 v = drawing->position.coords.x - 1;
            drawing->position.coords.x = v + (r & 3);
        }
        r = Item_NextRandom();
        {
            s32 v = drawing->position.coords.y - 1;
            drawing->position.coords.y = v + (r & 3);
        }
    }
    *(struct WirelessGamePair *)&sprite->x = drawing->position.words;
}

void sub_080399F4(struct WirelessGameItems *owner, struct WirelessGameItem *p, int index)
{
    if (p->entered) {
        p->entered = 0;
        Item_LoadFrame(owner, p, index);
        p->shakeTicks = 0;
        Item_Place(p, &p->sprite, &p->drawing, index);
    }
    if (p->count) {
        int slot = index & 3;
        int ticks = p->shakeTicks;
        if (ticks) {
            u8 *shakeTicks = &p->shakeTicks;
            *shakeTicks = ticks - 1;
            Item_Place(p, &p->sprite, &p->drawing, index);
        }
        if (p->timer > 29) {
            p->timer = 30;
            if (index > 7) {
                int sum = owner->current[slot] + owner->pending[slot];
                if (sum <= 0) {
                    p->state = 2;
                    p->entered = 1;
                    p->timer = 0;
                } else {
                    p->count = sum;
                }
            } else {
                p->state = 4;
                p->entered = 1;
                p->timer = 0;
            }
        }
    load:
        Item_LoadFrame(owner, p, index);
    } else {
        p->sprite.flags |= 1;
        p->drawing.flags |= 1;
    }
    p->timer++;
}

void WirelessGameItems_UpdateBlinkingItem(struct WirelessGameItems *p, struct WirelessGameItem *q,
                                          int index)
{
    if (q->entered)
        q->entered = 0;
    if (q->timer & 1) {
        int choice;
        void *draw;
        q->sprite.value = q->count;
        q->sprite.flags &= ~1;
        draw = &q->drawing;
        p = (void *)&p->resource;
        if (index > 7)
            choice = (index & 3) + 4;
        else
            choice = index & 3;
        Anim_LoadKeyframe(draw, p, gUnk_0824DE7C[choice], 0);
        q->drawing.flags &= ~1;
    } else {
        q->sprite.flags |= 1;
        q->drawing.flags |= 1;
    }
    if (q->timer > 9) {
        int one = 1;
        int zero = 0;
        q->state = one;
        q->entered = one;
        one = 0;
        q->timer = zero;
        q->count = one;
    }
    q->timer++;
}

extern const u16 gUnk_0824DE7C[], gUnk_0824DE8C[];

void Vec16_Lerp(void *, void *, void *, int, int);

// MATCHING_CONCESSION: register-pinning
void WirelessGameItems_UpdateItemMerge(struct WirelessGameItems *owner, struct WirelessGameItem *p,
                                       int indexArg)
{
    int index = indexArg;
    struct WirelessGameSprite *sprite = &p->sprite;
    struct WirelessGameDrawing *drawing = &p->drawing;
    if (p->entered) {
        int choice;
        int y;
        void *draw;
        void *resource;
        p->entered = 0;
        sprite->value = p->count;
        sprite->flags &= ~1;
        draw = drawing;
        resource = &owner->resource;
        if (index > 7)
            choice = (index & 3) + 4;
        else
            choice = index & 3;
        Anim_LoadKeyframe(draw, resource, gUnk_0824DE7C[choice], 0);
        p->drawing.flags &= ~1;
        {
            struct WirelessGameVector *end = &p->end;
            y = 128;
            if (index > 7)
                y = 144;
            p->end.x = gUnk_0824DE8C[index & 3] + 80;
            end->y = y;
            end->z = 0;
        }
        WirelessGameItem_SetRandomArcVelocity(p, 32, 15);
    }
    Vec16_Lerp(&drawing->position.words.x, &p->start, &p->end, p->timer, 5);
    WirelessGameItems_ApplyMotionWave(&drawing->position.words.x, p->timer, p->arcVelocity);
    *(struct WirelessGamePair *)&sprite->x = *(struct WirelessGamePair *)&drawing->position.words.x;
    if (p->timer > 31) {
        /* Keep the byte-offset temporary for the original register lifetime. */
        int offset = (index & ~4) * sizeof(struct WirelessGameItem) + (u32) &
                     ((struct WirelessGameItems *)0)->items;
        struct WirelessGameItem *target;
        int count;
        target = (struct WirelessGameItem *)((u8 *)owner + offset);
        count = p->count;
        if (target->state == 1)
            count += target->count;
        {
            int one = 1;
            int zero = 0;
            target->state = one;
            target->entered = one;
            target->timer = zero;
        }
        {
            register int value asm("r0") = count;
            if (value > 10)
                value = 10;
            else if (value < 0)
                value = 0;
            {
                int zero = 0;
                asm("" : "+r"(zero));
                target->count = value;
                value = 0;
                target->pendingFlag = value;
                target->shakeTicks = 8;
                p->state = zero;
                p->entered = 1;
                p->timer = zero;
                p->count = zero;
                p->pendingFlag = zero;
            }
        }
    }
    p->timer++;
}

// MATCHING_CONCESSION: register-pinning
void WirelessGameItems_UpdateOutgoingItem(struct WirelessGameItems *owner,
                                          struct WirelessGameItem *p, int index)
{
    struct WirelessGameSprite *sprite = &p->sprite;
    struct WirelessGameDrawing *drawing;
    int choice;
    {
        /* Existing pinned offset; derive it from the shared layout. */
        register int offset asm("r2") = (u32) & ((struct WirelessGameItem *)0)->drawing;
        struct WirelessGameDrawing *initial;
        asm("" : "+r"(offset));
        initial = (struct WirelessGameDrawing *)(offset + (u32)p);
        drawing = initial;
    }
    if (p->entered) {
        int zero = 0;
        struct WirelessGameVector *vector;
        register int y asm("r6");
        const u16 *table;
        const u16 *entry;
        p->entered = zero;
        vector = &p->start;
        y = 128;
        if (index > 7)
            y = 144;
        table = gUnk_0824DE8C;
        entry = (const u16 *)((index & 3) * 2 + (u32)table);
        p->start.x = *entry + 80;
        vector->y = y;
        vector->z = zero;
        vector = &p->end;
        y = 272;
        asm("" : "+r"(vector), "+r"(y));
        {
            register int x asm("r0") = y;
            int value;
            asm("" : "+r"(x));
            value = *entry;
            p->end.x = x + value;
        }
        vector->y = 32;
        vector->z = zero;
        WirelessGameItem_SetRandomNegativeArcVelocity(p, 32, 15);
    }
    p->sprite.value = p->count;
    p->sprite.flags &= ~1;
    {
        void *draw = &p->drawing;
        void *resource = &owner->resource;
        if (index > 7)
            choice = (index & 3) + 4;
        else
            choice = index & 3;
        Anim_LoadKeyframe(draw, resource, gUnk_0824DE7C[choice], 0);
    }
    p->drawing.flags &= ~1;
    {
        void *position = &drawing->position.words.x;
        Vec16_Lerp(position, &p->start, &p->end, p->timer, 5);
        WirelessGameItems_ApplyMotionWave(position, p->timer, p->arcVelocity);
    }
    {
        register struct WirelessGameDrawing *source asm("r2") = drawing;
        u32 x = source->position.words.x;
        u32 y = source->position.words.y;
        register struct WirelessGameSprite *dest asm("r6") = sprite;
        dest->x = x;
        dest->y = y;
    }
    if (p->timer > 31) {
        register int one asm("r1") = 1;
        int zero = 0;
        asm("" : "+r"(one), "+r"(zero));
        p->state = one;
        p->entered = one;
        p->timer = zero;
        p->count = zero;
        p->pendingFlag = zero;
    }
    p->timer++;
}

// MATCHING_CONCESSION: register-pinning
void WirelessGameItems_UpdateIncomingItem(struct WirelessGameItems *owner,
                                          struct WirelessGameItem *p, int indexArg)
{
    int index = indexArg;
    struct WirelessGameSprite *sprite;
    struct WirelessGameDrawing *drawing = &p->drawing;
    sprite = &p->sprite;
    if (p->entered) {
        int choice;
        int y;
        void *draw;
        void *resource;
        p->entered = 0;
        {
            int value = p->count;
            struct WirelessGameSprite *dest;
            dest = sprite;
            dest->value = value;
        }
        p->sprite.flags &= ~1;
        draw = drawing;
        resource = &owner->resource;
        if (index > 7)
            choice = (index & 3) + 4;
        else
            choice = index & 3;
        Anim_LoadKeyframe(draw, resource, gUnk_0824DE7C[choice], 0);
        p->drawing.flags &= ~1;
        {
            struct WirelessGameVector *start = &p->start;
            const u16 *entry;
            int x;
            int zero;
            struct WirelessGameVector *end;
            int y;
            {
                const u16 *table = gUnk_0824DE8C;
                int offset;
                offset = (index & 3) * 2;
                entry = (const u16 *)(offset + (u32)table);
            }
            {
                register int initial asm("r1") = 272;
                asm("" : "+r"(initial));
                x = initial;
            }
            {
                register int value asm("r1") = *entry;
                x += value;
            }
            zero = 0;
            p->start.x = x;
            start->y = 32;
            start->z = zero;
            end = &p->end;
            y = 128;
            if (index > 7)
                y = 144;
            p->end.x = *entry + 80;
            end->y = y;
            end->z = zero;
        }
        WirelessGameItem_SetRandomNegativeArcVelocity(p, 32, 15);
    }
    Vec16_Lerp(&drawing->position.words.x, &p->start, &p->end, p->timer, 5);
    WirelessGameItems_ApplyMotionWave(&drawing->position.words.x, p->timer, p->arcVelocity);
    {
        u32 x = drawing->position.words.x;
        u32 y = drawing->position.words.y;
        struct WirelessGameSprite *dest;
        dest = sprite;
        dest->x = x;
        dest->y = y;
    }
    if (p->timer > 31) {
        int slot = index & 3;
        u8 *current;
        register u8 *pending asm("r12");
        struct WirelessGameItem *target;
        int count;
        if (p->pendingFlag) {
            u8 *array = owner->pending;
            u8 *dest = array + slot;
            register int total asm("r1") = *dest;
            total += p->count;
            pending = array;
            if (total > 255)
                total = 255;
            *dest = total;
            current = owner->current;
        } else {
            u8 *dest;
            int total;
            current = owner->current;
            dest = current + slot;
            total = *dest;
            total += p->count;
            if (total > 255)
                total = 255;
            *dest = total;
            {
                int offset = (u32) & ((struct WirelessGameItems *)0)->pending;
                u8 *array;
                array = owner->pending;
                pending = array;
            }
        }
        {
            /* Keep the byte-offset temporary for the original register lifetime. */
            int offset = (index & ~4) * sizeof(struct WirelessGameItem) + (u32) &
                         ((struct WirelessGameItems *)0)->items;
            int first;
            int second;
            register int one asm("r0");
            int zero;
            target = (struct WirelessGameItem *)((u8 *)owner + offset);
            {
                u8 *address = current + slot;
                first = *address;
                current = pending;
                asm("" : "+r"(current));
                address = current + slot;
                second = *address;
            }
            one = 1;
            zero = 0;
            asm("" : "+r"(one), "+r"(zero));
            target->state = one;
            target->entered = one;
            target->timer = zero;
            {
                register int value asm("r0") = first + second;
                int clear;
                if (value > 10)
                    value = 10;
                clear = 0;
                asm("" : "+r"(clear));
                target->count = value;
                value = 0;
                target->pendingFlag = value;
                target->shakeTicks = 8;
                p->count = clear;
                p->state = clear;
                p->entered = 1;
                p->timer = clear;
            }
        }
    }
    p->timer++;
}

extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);

static inline int ScaleSine(int n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

static inline u32 WirelessGame_Random(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return ((const u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
}

static inline void WirelessGameItem_SetDrift(struct WirelessGameVector *drift, int angle)
{
    int k = 8;
    drift->x = ScaleSine(gUnk_086149C4[(angle + 64) & 255] * 8);
    drift->y = ScaleSine(gUnk_086149C4[angle & 255] * k);
    drift->z = 0;
}

void sub_0803A068(struct WirelessGameItems *owner, struct WirelessGameItem *p, int index)
{
    struct WirelessGameDrawing *drawing = &p->drawing;
    struct WirelessGameSprite *sprite = &p->sprite;
    if (p->entered) {
        struct WirelessGameVector *start;
        struct WirelessGameVector *end;
        const u16 *entry;
        int choice;
        s8 angle;
        p->entered = 0;
        sprite->value = p->count;
        p->sprite.flags &= ~1;
        {
            struct WirelessGameDrawing *d = drawing;
            struct WirelessGameResource *res = &owner->resource;
            if (index > 7)
                choice = (index & 3) + 4;
            else
                choice = index & 3;
            Anim_LoadKeyframe(d, res, gUnk_0824DE7C[choice], 0);
        }
        p->drawing.flags &= ~1;
        start = &p->start;
        {
            const u16 *table = gUnk_0824DE8C;
            entry = &table[index & 3];
        }
        p->start.x = 272 + *entry;
        start->y = 32;
        start->z = 0;
        end = &p->end;
        {
            int y = 128;
            if (index > 7)
                y = 144;
            p->end.x = *entry + 80;
            end->y = y;
            end->z = 0;
        }
        WirelessGameItem_SetRandomArcVelocity(p, 32, 15);
        angle = Math_Atan2(p->start.x - p->end.x, start->y - end->y);
        {
            u32 r = WirelessGame_Random();
            s32 t = angle - 16;
            t += r & 31;
            WirelessGameItem_SetDrift(&p->drift, (s8)t);
        }
    }
    if (p->timer <= 27) {
        void *position = &drawing->position.words.x;
        Vec16_Lerp(position, &p->start, &p->end, p->timer, 5);
        WirelessGameItems_ApplyMotionWave(position, p->timer, p->arcVelocity);
    } else {
        struct WirelessGamePosition *pos = &drawing->position.coords;
        struct WirelessGameVector *drift = &p->drift;
        pos->x += drift->x;
        pos->y += drift->y;
        pos->z += drift->z;
    }
    *(struct WirelessGamePair *)&sprite->x = *(struct WirelessGamePair *)&drawing->position.words.x;
    if (p->timer > 43) {
        int zero = 0;
        p->state = zero;
        p->entered = 1;
        p->timer = zero;
    }
    p->timer++;
}

void WirelessGameItems_UpdateTemporaryItem(struct WirelessGameItems *p, struct WirelessGameItem *q,
                                           int index)
{
    void *alias;
    /* do-while(0) only keeps agbcc's block layout */
    do {
        if (q->entered) {
            int choice;
            void *draw;
            q->entered = 0;
            q->sprite.value = q->count;
            q->sprite.flags &= ~1;
            draw = &q->drawing;
            p = (void *)&p->resource;
            if (index > 7) {
                choice = (index & 3) + 4;
            } else {
                choice = index & 3;
            }
            Anim_LoadKeyframe(alias = draw, p, gUnk_0824DE7C[choice], 0);
            q->drawing.flags &= ~1;
        }
        if (q->timer > 31) {
            q->state = 0;
            q->entered = 1;
            q->timer = 0;
        }
        q->timer++;
    } while (0);
}

extern const u8 gUnk_0824DE94[];
extern const struct WirelessGameVector gUnk_03005418;

void Widget_InitLinked(void *, void *, int, int, int, int, int, const void *);

void WirelessGameItems_InitItem(struct WirelessGameItems *owner, struct WirelessGameItem *p,
                                int index)
{
    int state;
    int choice;
    int zero;
    struct WirelessGameSprite *sprite;
    if ((u32)index <= 3 || (u32)(index - 8) <= 3)
        state = 1;
    else
        state = 0;
    zero = 0;
    p->count = zero;
    p->pendingFlag = zero;
    sprite = &p->sprite;
    Sprite_Init(sprite, owner->sprite, 0x10011);
    {
        int clear = 0;
        u8 *visible;
        sprite->value = zero;
        visible = &p->sprite.visible;
        *visible = clear;
    }
    if (index > 7)
        choice = (index & 3) + 4;
    else
        choice = index & 3;
    {
        int animation = gUnk_0824DE7C[choice];
        Widget_InitLinked(&p->drawing, &owner->resource, animation, 17, 0, 0, 60, gUnk_0824DE94);
    }
    p->drawing.changed = 1;
    p->state = state;
    p->entered = 1;
    p->timer = 0;
}

void WirelessGameItems_DestroyItem(struct WirelessGameItems *a, struct WirelessGameItem *p,
                                   int index)
{
    Sprite_Remove(&p->sprite);
    Anim_Remove(&p->drawing);
}

s32 SceneEffectQueue_QueueTypeZeroEffect(void);

s32 sub_081998B0(void);

s32 SceneEffectQueue_QueueTypeTwoEffect(void);

s32 Vampire_TryQueueCommand(u32);

void WirelessGameItems_PublishPendingEffects(struct WirelessGameItems *p)
{
    if (p->current[2] != 0) {
        if (SceneEffectQueue_QueueTypeZeroEffect() != 0)
            p->current[2]--;
    }
    if (p->current[1] != 0) {
        if (sub_081998B0() != 0)
            p->current[1]--;
    }
    if (p->current[3] != 0) {
        if (SceneEffectQueue_QueueTypeTwoEffect() != 0)
            p->current[3]--;
    }
    if (p->current[0] != 0) {
        if (Vampire_TryQueueCommand(0xb4) != 0)
            p->current[0]--;
    }
}

extern void *gUnk_086057DC[];

int Wireless_GetStatus(void);

void sub_08249244(void *, void *, int, void *);

int WirelessGameItems_Update(struct WirelessGameItems *p)
{
    if (!Wireless_GetStatus()) {
        struct WirelessGameItem *q = p->items;
        int i = 0;
        do {
            sub_08249244(p, q, i, gUnk_086057DC[q->state]);
            i++;
            q++;
        } while (i <= 15);
        WirelessGameItems_PublishPendingEffects(p);
    }
    return 0;
}

int WirelessGameItems_Destroy(struct WirelessGameItems *p)
{
    struct WirelessGameItem *q = p->items;
    int i = 0;
    do {
        WirelessGameItems_DestroyItem(p, q, i);
        i++;
        q++;
    } while (i <= 15);
    return (int)(gWirelessGameItems = 0);
}

int WirelessGameItems_Init(struct WirelessGameItems *p)
{
    struct WirelessGameResource *resource;
    struct WirelessGameItem *item;
    u8 *current;
    u8 *pending;
    int i;
    gWirelessGameItems = p;
    resource = ScriptProc_Start(0xcb05, 0x5d04);
    if (!resource)
        return -1;
    p->resource = *resource;
    ResourceHeader_Relocate(&p->resource, resource);
    if (!AnimData_Load(p->sprite, 0x18a9))
        return -1;
    item = p->items;
    i = 0;
    current = p->current;
    pending = p->pending;
    do {
        WirelessGameItems_InitItem(p, item, i);
        i++;
        item++;
    } while (i <= 15);
    i = 0;
    {
        u8 *first = current;
        int zero = 0;
        u8 *second = pending;
        do {
            first[i] = zero;
            second[i] = zero;
        } while (++i <= 3);
    }
    return 0;
}

void *WirelessGameItems_Create(int unused0, int unused1)
{
    void *p = Task_Create(11, 0xb5c);
    if (p) {
        Task_SetCallbacks(p, WirelessGameItems_Update, WirelessGameItems_Destroy);
        if (WirelessGameItems_Init(p) >= 0)
            goto ok;
        Task_Kill(p);
        return 0;
    }
ok:
    return p;
}

// MATCHING_CONCESSION: register-pinning
int WirelessGameItems_QueueLocalItem(struct WirelessGameVector *position, int index, int amount)
{
    struct WirelessGameItems *owner = gWirelessGameItems;
    struct WirelessGameItem *target;
    if (!owner)
        return -1;
    if (amount > 0) {
        /* Field-derived byte offsets preserve the matched address lifetimes. */
        int size = sizeof(struct WirelessGameItem);
        int scaled = index * size;
        int extra = (u32) & ((struct WirelessGameItems *)0)->items[4];
        int offset = scaled + extra;
        target = (struct WirelessGameItem *)((u8 *)owner + offset);
        if (target->count) {
            struct WirelessGameItem *current;
            int offset = scaled + (u32) & ((struct WirelessGameItems *)0)->items;
            int count;
            current = (struct WirelessGameItem *)((u8 *)owner + offset);
            if (current->state == 4)
                count = target->count;
            else
                count = current->count + target->count;
            {
                int one = 1, zero = 0;
                current->state = one;
                current->entered = one;
                current->timer = zero;
            }
            {
                register int value asm("r0") = count;
                int clear;
                if (value > 10)
                    value = 10;
                else if (value < 0)
                    value = 0;
                clear = 0;
                current->count = value;
                value = 0;
                current->pendingFlag = value;
                target->count = clear;
            }
        }
        if (amount > 10)
            amount = 10;
        else if (amount < 0)
            amount = 0;
        {
            int zero = 0;
            register int flag asm("r0");
            target->count = amount;
            flag = 0;
            target->pendingFlag = flag;
            target->state = 3;
            target->entered = 1;
            target->timer = zero;
        }
        {
            struct WirelessGameVector *start = &target->start;
            int x = position->x;
            int z = position->z;
            int v = (x - z) * 3;
            int y, height;
            if (v >= 0)
                v >>= 5;
            else
                v = -((-v) >> 5);
            start->x = v;
            v = (x + z) * 3;
            if (v >= 0)
                v >>= 5;
            else
                v = -((-v) >> 5);
            y = v;
            v = position->y * 3;
            if (v >= 0)
                v >>= 5;
            else
                v = -((-v) >> 5);
            height = v;
            start->y = y - height;
            start->z = y + height;
            start->x = start->x - gUnk_03005418.x + 120;
            start->y = (y - height) - gUnk_03005418.y + 80;
            start->z = (y + height) - gUnk_03005418.z;
        }
    }
    return 0;
}

// MATCHING_CONCESSION: register-pinning
int WirelessGameItems_QueueIncomingItem(int indexArg, int amountArg, int pendingArg)
{
    int index = indexArg;
    int amount = amountArg;
    int pendingFlag = pendingArg;
    struct WirelessGameItems *owner = gWirelessGameItems;
    struct WirelessGameItem *target;
    if (!owner)
        return -1;
    if (amount <= 0) {
        int result = 0;
        return result;
    }
    {
        /* Field-derived byte offsets preserve the matched address lifetimes. */
        int size = sizeof(struct WirelessGameItem);
        int scaled;
        int extra;
        int offset;
        scaled = index * size;
        extra = (u32) & ((struct WirelessGameItems *)0)->items[12];
        offset = scaled + extra;
        target = (struct WirelessGameItem *)((u8 *)owner + offset);
        if (target->count) {
            struct WirelessGameItem *current;
            register u8 *first asm("r1");
            u8 *second;
            register u8 *dest asm("r5");
            int total;
            extra = (u32) & ((struct WirelessGameItems *)0)->items[8];
            offset = scaled + extra;
            current = (struct WirelessGameItem *)((u8 *)owner + offset);
            if (target->pendingFlag) {
                first = owner->pending;
                {
                    int slot = index;
                    dest = first + slot;
                }
                total = *dest;
                total += target->count;
                second = first;
                if (total > 255)
                    total = 255;
                *dest = total;
                first = owner->current;
            } else {
                first = owner->current;
                {
                    int slot = index;
                    dest = first + slot;
                }
                total = *dest;
                total += target->count;
                if (total > 255)
                    total = 255;
                *dest = total;
                second = owner->pending;
            }
            {
                int slot = index;
                register u8 *address asm("r0");
                int a;
                int b;
                register int one asm("r0");
                int zero;
                asm("" : "+r"(slot));
                address = first + slot;
                a = *address;
                address = second + slot;
                b = *address;
                one = 1;
                zero = 0;
                asm("" : "+r"(a), "+r"(b), "+r"(one), "+r"(zero));
                current->state = one;
                current->entered = one;
                current->timer = zero;
                {
                    register int value asm("r0") = a + b;
                    int clear;
                    if (value > 10)
                        value = 10;
                    clear = 0;
                    asm("" : "+r"(clear));
                    current->count = value;
                    value = 0;
                    current->pendingFlag = value;
                    target->count = clear;
                }
            }
        }
    }
    {
        int value = amount;
        int zero;
        if (value > 10)
            value = 10;
        else if (value < 0)
            value = 0;
        zero = 0;
        target->count = value;
        value = pendingFlag;
        target->pendingFlag = value;
        target->state = 5;
        target->entered = 1;
        target->timer = zero;
        return zero;
    }
}

void WirelessGameItems_StartOutgoingItems(void)
{
    struct WirelessGameItems *p = gWirelessGameItems;
    if (p) {
        int i = 0;
        int state = 4;
        int zero = 0;
        u8 *count = p->pending;
        do {
            if (*count) {
                int value;
                /* Direct items[i] indexing changes agbcc register allocation. */
                int offset = i * sizeof(struct WirelessGameItem) + (u32) &
                             ((struct WirelessGameItems *)0)->items;
                struct WirelessGameItem *q;
                q = (struct WirelessGameItem *)(((u8 *)p) + (*(&offset)));
                q->state = state;
                do {
                    q->entered = 1;
                    q->timer = zero;
                } while (0);
                value = *count;
                {
                    int clear = 0;
                    if (value > 10) {
                        value = 10;
                    }
                    q->count = value;
                    q->pendingFlag = clear;
                }
                *count = zero;
            }
            count++;
            i++;
        } while (i <= 3);
    }
}

void WirelessGameDisplay_UpdateHidden(struct WirelessGameDisplay *p)
{
    if (p->entered) {
        struct WirelessGameDrawing *q;
        int i;
        u32 mask;
        p->entered = 0;
        q = p->drawings;
        mask = 1;
        i = 3;
        do {
            q->flags |= mask;
            i--;
            q++;
        } while (i >= 0);
    }
    if (p->finished)
        p->finished = 0;
}

void WirelessGameDisplay_UpdateStaticNotice(struct WirelessGameDisplay *p)
{
    if (p->entered) {
        struct WirelessGameDrawing *q;
        p->entered = 0;
        q = p->drawings;
        Anim_LoadKeyframe(q, &p->resource, 26, 0);
        q->flags &= ~1;
    }
    if (p->finished) {
        p->finished = 0;
        p->state = 0;
        p->entered = 1;
        p->timer = 0;
    } else
        p->timer++;
}

// MATCHING_CONCESSION: register-pinning
void WirelessGameDisplay_UpdateAlternatingNotice(struct WirelessGameDisplay *p)
{
    if (p->entered) {
        struct WirelessGameDrawing *q;
        p->entered = 0;
        q = p->drawings;
        Anim_LoadKeyframe(q, &p->resource, 25, 0);
        q->flags &= ~1;
    }
    {
        u32 phase = p->timer & 127;
        if (phase == 63) {
            register struct WirelessGameDrawing *q asm("r4") = p->drawings;
            Anim_LoadKeyframe(q, &p->resource, 24, 0);
        } else if (phase == 127) {
            register struct WirelessGameDrawing *q asm("r4") = p->drawings;
            Anim_LoadKeyframe(q, &p->resource, 25, 0);
        }
    }
    if (p->finished) {
        p->finished = 0;
        p->state = 0;
        p->entered = 1;
        p->timer = 0;
    } else
        p->timer++;
}

int Div(int, int);

int Mod(int, int);

// MATCHING_CONCESSION: register-pinning
void WirelessGameDisplay_UpdateRoundAnnouncement(struct WirelessGameDisplay *p)
{
    struct WirelessGameDrawing *q;
    void *volatile resource;
    u16 *volatile table;
    register int mask asm("r8");
    if (p->entered) {
        register int count asm("r4");
        p->entered = 0;
        count = WirelessGame_GetRoundCount();
        if (count == 1)
            Sound_PlaySong(706);
        q = p->drawings;
        resource = &p->resource;
        Anim_LoadKeyframe(q, resource, 11, 0);
        {
            u32 flags = q->flags;
            int value = ~1;
            mask = value;
            q->flags = flags & value;
        }
        q++;
        Anim_LoadKeyframe(q, resource, (u16)(Div(count, 10) + 12), 0);
        {
            u32 flags = q->flags;
            register int value asm("r3");
            value = mask;
            asm("" : "+r"(value));
            q->flags = flags & value;
        }
        q = &p->drawings[2];
        Anim_LoadKeyframe(q, resource, (u16)(Mod(count, 10) + 12), 0);
        q->flags &= mask;
        q->position.coords.x = 16;
        {
            register int offset asm("r3") = (u32) & ((struct WirelessGameDisplay *)0)->drawings[3];
            asm("" : "+r"(offset));
            q = (struct WirelessGameDrawing *)((u8 *)p + offset);
        }
        Anim_LoadKeyframe(q, resource, 29, 0);
        q->flags |= 1;
    }
    if (p->timer == 90) {
        q = p->drawings;
        Anim_LoadKeyframe(q, &p->resource, 9, 0);
        q->flags &= ~1;
        q++;
        q->flags |= 1;
        q = &p->drawings[2];
        q->flags |= 1;
        {
            register int offset asm("r3") = (u32) & ((struct WirelessGameDisplay *)0)->drawings[3];
            asm("" : "+r"(offset));
            q = (struct WirelessGameDrawing *)((u8 *)p + offset);
        }
        q->flags |= 1;
        Sound_PlaySong(1452);
    }
    if (p->timer - 90 <= 7) {
        register u32 *state asm("r2");
        register u32 limit asm("r5");
        register u32 index asm("r0");
        int value;
        int bits;
        q = p->drawings;
        table = gUnk_0203B400;
        state = &gRandomIndex;
        index = *state + 1;
        limit = 1023;
        index &= limit;
        *state = index;
        value = index * 2;
        value = *(u16 *)(value + (u32)table);
        bits = 3;
        value &= bits;
        value -= 2;
        q->position.coords.x = value;
        index = (index + 1) & limit;
        *state = index;
        index *= 2;
        index = *(u16 *)(index + (u32)table) & bits;
        index -= 2;
        q->position.coords.y = index;
    }
    if (p->timer == 98) {
        q = p->drawings;
        q->position.coords.x = 0;
        q->position.coords.y = 0;
    }
    if (p->finished) {
        p->finished = 0;
        p->state = 0;
        p->entered = 1;
        p->timer = 0;
    } else
        p->timer++;
}

// MATCHING_CONCESSION: register-pinning
void WirelessGameDisplay_UpdateShakeAnimation(struct WirelessGameDisplay *arg)
{
    register struct WirelessGameDisplay *p asm("r6") = arg;
    struct WirelessGameDrawing *q;
    if (p->entered) {
        p->entered = 0;
        q = p->drawings;
        Anim_LoadKeyframe(q, &p->resource, 9, 0);
        q->flags &= ~1;
        Sound_PlaySong(1452);
    }
    if (p->timer <= 7) {
        u16 *table;
        u32 *state;
        u32 limit;
        register u32 index asm("r0");
        int value;
        int mask;
        q = p->drawings;
        table = gUnk_0203B400;
        state = &gRandomIndex;
        index = *state + 1;
        limit = 1023;
        index &= limit;
        *state = index;
        value = table[index];
        mask = 3;
        value &= mask;
        value -= 2;
        q->position.coords.x = value;
        index = (index + 1) & limit;
        *state = index;
        index = table[index] & mask;
        index -= 2;
        q->position.coords.y = index;
    }
    if (p->timer == 8) {
        q = p->drawings;
        q->position.coords.x = 0;
        q->position.coords.y = 0;
    }
    if (p->finished) {
        p->finished = 0;
        p->state = 0;
        p->entered = 1;
        p->timer = 0;
    } else
        p->timer++;
}

void WirelessGameDisplay_UpdateChimeAnimation(struct WirelessGameDisplay *p)
{
    if (p->entered) {
        struct WirelessGameDrawing *q;
        p->entered = 0;
        q = p->drawings;
        Anim_LoadKeyframe(q, &p->resource, 10, 0);
        q->flags &= ~1;
        Sound_PlaySong(660);
    }
    if (p->finished) {
        p->finished = 0;
        p->state = 0;
        p->entered = 1;
        p->timer = 0;
    } else
        p->timer++;
}

extern void *gUnk_086057FC[];

int WirelessGameDisplay_Update(struct WirelessGameDisplay *p)
{
    if (!Wireless_GetStatus())
        Callback_InvokeActor(p, gUnk_086057FC[p->state]);
    return 0;
}

int WirelessGameDisplay_Destroy(struct WirelessGameDisplay *p)
{
    struct WirelessGameDrawing *q = p->drawings;
    int i = 3;
    do {
        Anim_Remove(q);
        i--;
        q++;
    } while (i >= 0);
    q = p->extra;
    i = 1;
    do {
        Anim_Remove(q);
        q++;
        i--;
    } while (i >= 0);
    return 0;
}

int WirelessGameDisplay_Init(struct WirelessGameDisplay *p)
{
    struct WirelessGameResource *resource;
    struct WirelessGameDrawing *drawing;
    int i;
    struct WirelessGameVector position;
    resource = ScriptProc_Start(0xcb05, 0x7c8b);
    if (!resource)
        return -1;
    p->resource = *resource;
    ResourceHeader_Relocate(&p->resource, resource);
    for (i = 0, drawing = p->drawings; i < 4; i++, drawing++)
        Widget_InitLinked(drawing, &p->resource, 24, 17, 0, 0, 60, 0);
    resource = ScriptProc_Start(0xcb05, 0x5d04);
    if (!resource)
        return -1;
    p->extraResource = *resource;
    ResourceHeader_Relocate(&p->extraResource, resource);
    for (i = 0; i < 2; i++) {
        int animation;
        if (i == 0) {
            position.x = 132;
            animation = 405;
        } else {
            position.x = 220;
            animation = 408;
        }
        position.y = 24;
        position.z = 0;
        Widget_InitLinked(&p->extra[i], &p->extraResource, animation, 0x10010, 0, 0, 60, &position);
    }
    p->state = 0;
    p->entered = 1;
    p->timer = 0;
    p->finished = 0;
    return 0;
}

void *WirelessGameDisplay_Create(void)
{
    void *p = Task_Create(11, 672);
    if (p) {
        Task_SetCallbacks(p, WirelessGameDisplay_Update, WirelessGameDisplay_Destroy);
        if (WirelessGameDisplay_Init(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
