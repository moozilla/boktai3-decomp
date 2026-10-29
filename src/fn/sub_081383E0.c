// MATCHING_CONCESSION: register-pinning
#include "global.h"

/* Result-counter meanings are tentative; the score formula is established. */
struct SessionResults {
    u8 unk_00[0x8C];
    u16 positive[4];
    u16 negative[4];
};
struct SessionScores {
    u8 unk_00[0x1A];
    u16 participantCount;
    u8 unk_1C[0x84 - 0x1C];
    s16 points[4];
    s16 totals[4];
    s32 awardScale;
    s32 awards[4];
    s16 rankBonus[4];
    u8 unk_B0[0xE0 - 0xB0];
    struct SessionResults results;
};
s32 LinkSession_CountPlayerKind(s32 kind);
static inline void MarkAbsent(u16 *entry, u32 mask)
{
    u32 value = *entry;
    value |= mask;
    *entry = value;
}

void Registry1745_UpdateParticipantScores(struct SessionScores *session)
{
    struct SessionResults *results = &session->results;
    s32 i = 0;
    for (; i < session->participantCount; i++) {
        s32 total;
        session->points[i] = LinkSession_CountPlayerKind(i) * 16 + results->positive[i] * 9 -
                             results->negative[i] * 4;
        if (session->points[i] < 0)
            session->points[i] = 0;
        if (session->points[i] > 9999)
            session->points[i] = 9999;
        total = session->totals[i] + session->points[i];
        if (total > 9999)
            session->totals[i] = 9999;
        else
            session->totals[i] = total;
    }
    if (i <= 3) {
        register u32 initialMask asm("r1") = 0xFFFF;
        u32 mask;
        asm("" : "+r"(initialMask));
        mask = initialMask;
        do {
            MarkAbsent((u16 *)&session->points[i], mask);
            MarkAbsent((u16 *)&session->totals[i], mask);
        } while (++i <= 3);
    }
}
s32 Div(s32, s32);
extern const s8 gUnk_086108C8[];
#define Registry1745_UpdateRankingAwards sub_081384A0
void Registry1745_UpdateRankingAwards(struct SessionScores *session)
{
    s32 order[4], values[4], groupSizes[4], ranks[4];
    s32 i, j, group;
    for (i = 0; i < session->participantCount; i++) {
        order[i] = i;
        values[i] = session->points[i];
    }
    for (; i <= 3; i++) {
        order[i] = -1;
        values[i] = -1;
    }
    for (i = 0; i <= 2; i++) {
        /* Keep the pass counter separate from the inner scan. */
        asm("" : "+r"(i));
        for (j = 3; j > i; j--) {
            if (values[j - 1] < values[j]) {
                s32 value = values[j - 1];
                s32 index;
                s32 nextValue = values[j];
                values[j] = value;
                values[j - 1] = nextValue;
                index = order[j];
                order[j] = order[j - 1];
                order[j - 1] = index;
            }
        }
    }
    {
        s32 k;
        for (k = 0; k < 4; k++)
            groupSizes[k] = 0;
    }
    group = 0;
    groupSizes[0] = 1;
    ranks[0] = 0;
    for (i = 1; i < session->participantCount; i++) {
        if (values[i] < values[i - 1])
            group = i;
        ranks[i] = group;
        groupSizes[group]++;
    }
    for (i = 0; i < session->participantCount; i++) {
        s32 index = order[i];
        s32 rank = ranks[i];
        s32 award = session->rankBonus[rank];
        s32 amount =
            Div(session->awardScale * gUnk_086108C8[rank + session->participantCount * 4], 20);
        if (amount >= 0)
            amount >>= 6;
        else
            amount = -((-amount) >> 6);
        if (amount > 0) {
            if (amount > 60)
                amount = 60;
        } else if (amount < -60) {
            amount = -60;
        }
        award += amount;
        if (groupSizes[rank] > 1)
            award = Div(award, groupSizes[rank]);
        session->awards[index] += award;
    }
}
