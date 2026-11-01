// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "contact_swarm.h"
#include "player_cue_sequence.h"

// confidence=low: foreground role inferred from screen-coordinate placement and its refresh caller.
struct Drawable {
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    u32 unknown3C, ended, mode;
};
static inline u32 StepPlayback(struct Drawable *owner)
{
    return PlayerCue_StepSequenceEnded(&owner->sprite, &owner->sequence);
}
void ActorDisplay_UpdateForegroundSprite(u8 *input)
{
    u8 *p = input;
    struct Drawable *draw = (struct Drawable *)(p + 0x2C0);
    u32 off1;
    u32 off2;
    u16 *origin;
    register u16 *screen asm("r0");
    register u32 x asm("r1");
    u32 y;
    u32 result;
    draw->ended = 0;
    off1 = 0x278;
    screen = (u16 *)(p + off1);
    off2 = 0x5E0;
    origin = (u16 *)(p + off2);
    x = *origin + 42;
    x += *screen;
    off2 += 6;
    screen = (u16 *)(p + off2);
    result = *screen + x;
    draw->sprite.position.x = result;
    off1 = 0x288;
    y = *(s32 *)(p + off1);
    off1 += 8;
    screen = (u16 *)(p + off1);
    result = *(s32 *)screen + 74;
    y += result;
    x = *(s32 *)(p + 0x5A0);
    x += y;
    x += *origin;
    off2 = 0x5E8;
    result = *(u16 *)(p + off2) + x;
    draw->sprite.position.y = result;
    if (p[0xAE] != 9) {
        if (p[0xB3]) {
            off1 = 0x5E4;
            screen = (u16 *)(p + off1);
            if (*(s16 *)screen)
                draw->sprite.flags |= 1;
            else
                draw->sprite.flags &= ~1;
        } else
            draw->sprite.flags &= ~1;
    }
    if (draw->mode != 1)
        draw->ended = StepPlayback(draw);
}
