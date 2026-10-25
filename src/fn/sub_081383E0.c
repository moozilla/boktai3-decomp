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
    u8 unk_94[0xE0 - 0x94];
    struct SessionResults results;
};
s32 LinkSession_CountPlayerKind(s32 kind);
static inline void MarkAbsent(u16 *entry, u32 mask)
{
    u32 value = *entry;
    value |= mask;
    *entry = value;
}

#ifdef NONMATCHING
// fuzzy: 49.0%
// scope shape 197 (differences once low registers are set aside)
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
#else
INCLUDE_ASM("asm/nonmatching", sub_081383E0);
#endif
