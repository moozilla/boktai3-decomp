// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reset.h"
extern u8 *gUnk_02000580;
void ActorBehavior_TickParticipantProgress(void);
void SceneActorControl_OnIdle(void);
void SceneActorControl_ResetTargetFlags(void *);
struct Pair { u32 xy, z; };
static inline u8 Take(u8 *p) { if (*p) { *p = 0; return 1; } return 0; }
static inline void Control(struct SceneActorRegistryResetView *s, void *fn, u32 code)
{
    u8 *queued = &s->field2B2;
    u32 zero = 0;
    *queued = 1;
    s->field2B0 = zero;
    s->field2B5 = code;
    s->target = fn;
    s->field2C8 = zero;
    { u32 *flags = &s->flags114; u32 mask = ~1; *flags &= mask; }
    s->word104 = zero;
}
static inline void Delta(struct ContactPosition *out, struct ContactPosition *player, struct ContactPosition *saved)
{
    out->x = player->x - saved->x;
    out->y = player->y - saved->y;
    out->z = player->z - saved->z;
}
// confidence=low: follows displacement of the player during participant-progress control.
#define Lun_FollowPlayerDisplacement sub_0809D7C4
#ifdef NONMATCHING
void Lun_FollowPlayerDisplacement(struct SceneActorRegistryResetView *s)
{
    register u8 *context asm("r7") = s->context;
    asm("" : "+r"(context));
    struct ContactPosition delta;
    if (Take(&s->field2B1)) {
        Control(s, ActorBehavior_TickParticipantProgress, 0x23);
        *(struct Pair *)(context + 0x414) = *(struct Pair *)(gUnk_02000580 + 0x30);
        SceneActorControl_ResetTargetFlags(s);
    }
    {
        struct ContactPosition *player = (void *)(gUnk_02000580 + 0x30);
        struct ContactPosition *saved = (void *)(context + 0x414);
        Delta(&delta, player, saved);
        s->motion.x += delta.x;
        s->motion.z += delta.z;
        *(struct Pair *)saved = *(struct Pair *)player;
    }
    if (s->field2B0) {
        u32 zero = 0;
        s->state2A8 = zero;
        s->phase2A9 = zero;
        s->behavior2AA = 3;
        s->counter2AC = zero;
        s->field2B1 = 1;
        Control(s, SceneActorControl_OnIdle, 0x10);
    } else s->counter2AC++;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0809D7C4);
#endif
