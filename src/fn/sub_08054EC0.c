// MATCHING_CONCESSION: register-pinning
#include "display_track.h"

void DisplayTrack_Update(struct DisplayTrack *track)
{
    if (!track->flags)
        return;
    if (track->flags & 1) {
        s32 left;
        track->moveFrame++;
        left = track->moveDuration - track->moveFrame;
        track->sprite.position.x =
            Div(track->origin.x * left + track->target.x * track->moveFrame, track->moveDuration);
        track->sprite.position.y =
            Div(track->origin.y * left + track->target.y * track->moveFrame, track->moveDuration);
        track->sprite.position.z =
            Div(track->origin.z * left + track->target.z * track->moveFrame, track->moveDuration);
        if (track->moveFrame >= track->moveDuration)
            track->flags &= ~1;
    }
    if (track->flags & 2) {
        u16 *duration;
        track->paletteFrame++;
        duration = &track->paletteDuration;
        if (track->paletteFrame >= *duration) {
            AnimData_SelectPaletteBank(&track->graphics, track->paletteBase + track->paletteEnd);
            track->flags &= ~2;
        } else {
            s32 i;
            const u16 *start =
                (const u16 *)gPaletteBankData + (track->paletteBase + track->paletteStart) * 16;
            const u16 *end =
                (const u16 *)gPaletteBankData + (track->paletteBase + track->paletteEnd) * 16;
            s32 left = *duration - track->paletteFrame;
            u16 *savedFrame;
            u16 *defaultPalette;
            u16 *initial;
            u16 *out;
            register u16 *period asm("r9");
            initial = track->blendedPalette;
            defaultPalette = &track->graphics.defaultPalette;
            savedFrame = &track->paletteFrame;
            period = duration;
            out = initial;
            i = 15;
            do {
                const u16 *read = start;
                u32 startColor;
                u32 redStart;
                u32 greenStart, blueStart;
                register u32 channelMask asm("r1");
                u32 endColor;
                u32 redEnd;
                u32 greenEnd, blueEnd;
                s32 redBlend, green, blue;
                startColor = *read;
                redStart = 31;
                redStart &= startColor;
                greenStart = ((startColor << 16) >> 21);
                channelMask = 31;
                greenStart &= channelMask;
                blueStart = ((startColor << 16) >> 26) & channelMask;
                endColor = *end;
                redEnd = 31;
                redEnd &= endColor;
                greenEnd = ((endColor << 16) >> 21) & channelMask;
                blueEnd = ((endColor << 16) >> 26) & channelMask;
                asm("" : : "r"(greenEnd));
                {
                    s32 sum = redStart * left + redEnd * *savedFrame;
                    register u16 *divisor asm("r2") = period;
                    redBlend = Div(sum, *divisor);
                }
                {
                    u32 first;
                    asm("" : : "r"(greenStart));
                    {
                        register s32 factor asm("r1") = left;
                        asm("" : : "r"(factor));
                        first = greenStart * factor;
                    }
                    {
                        register u16 *frame asm("r2") = savedFrame;
                        s32 sum;
                        asm("" : : "r"(frame));
                        sum = first + greenEnd * *frame;
                        {
                            register u16 *divisor asm("r3") = period;
                            green = Div(sum, *divisor);
                        }
                    }
                }
                {
                    u32 first;
                    {
                        register s32 factor asm("r1") = left;
                        asm("" : : "r"(factor));
                        first = blueStart * factor;
                    }
                    {
                        register u16 *frame asm("r2") = savedFrame;
                        s32 sum;
                        asm("" : : "r"(frame));
                        sum = first + blueEnd * *frame;
                        {
                            register u16 *divisor asm("r3") = period;
                            blue = Div(sum, *divisor);
                        }
                    }
                }
                {
                    u32 pixel = (blue << 10) | (green << 5);
                    {
                        register u32 red asm("r1") = redBlend;
                        asm("" : "+r"(red) : "r"(pixel));
                        pixel |= red;
                    }
                    {
                        u16 *destination = out;
                        *destination = pixel;
                    }
                }
                {
                    const u16 *next = start + 1;
                    start = next;
                }
                {
                    const u16 *next = end + 1;
                    end = next;
                }
                {
                    u32 step = 2;
                    out = (u16 *)((u8 *)out + step);
                }
                {
                    register s32 remaining asm("r2") = i - 1;
                    i = remaining;
                    if (remaining < 0)
                        break;
                }
            } while (1);
            {
                register u16 *palette asm("r3") = initial;
                asm("" : : "r"(palette));
                track->graphics.palette = palette;
            }
            {
                register u32 base asm("r1") = 600;
                register u32 value asm("r0");
                register u32 tag asm("r2");
                u16 *destination;
                asm("" : "+r"(base));
                value = base;
                asm("" : "+r"(value));
                tag = track->tag;
                value += tag;
                destination = defaultPalette;
                *destination = value;
            }
        }
    }
    if (track->flags & 4) {
        /* Keep startColor scratch value live across the comparison to steer its reload. */
        register u32 comparisonScratch asm("r2");
        if (++track->scaleFrame >= track->scaleDuration) {
            {
                u32 end = track->scaleEnd;
                track->sprite.unknown8[0] = end;
                track->sprite.unknown8[1] = end;
                if ((s8)track->sprite.unknown8[0] == 64)
                    track->sprite.flags &= ~2;
                track->flags &= ~4;
            }
        } else {
            u32 scale;
            asm("" : : "r"(comparisonScratch));
            track->sprite.flags |= 2;
            scale = Div(track->scaleStart * (track->scaleDuration - track->scaleFrame) +
                            track->scaleEnd * track->scaleFrame,
                        track->scaleDuration);
            track->sprite.unknown8[0] = scale;
            track->sprite.unknown8[1] = scale;
        }
    }
    if (track->flags & 8) {
        struct ContactSwarmSprite *sprite = &track->sprite;
        struct DisplaySequenceState *state = &track->sequenceState;
        const u16 *command = &state->commands[state->index];
        u32 done;
        sprite->frame = *command >> 6;
        {
            register u32 raw asm("r0") = state->flags;
            register u32 bit asm("r3");
            register u32 flags asm("r1");
            bit = 1;
            flags = bit;
            flags &= raw;
            if (flags != (((*command & 0x30) >> 4) & bit))
                track->sprite.flags |= 4;
            else
                track->sprite.flags &= ~4;
            raw = state->flags;
            bit = 2;
            flags = bit;
            flags = (u8)(flags & raw);
            if (flags != (((*command & 0x30) >> 4) & bit))
                sprite->flags |= 8;
            else
                sprite->flags &= ~8;
        }
        {
            u32 elapsed = state->elapsed + 1;
            u32 mask;
            register u32 duration asm("r2");
            u32 reverseDuration;
            state->elapsed = elapsed;
            {
                register u32 literal asm("r2") = 0xFFFF;
                asm("" : : "r"(literal));
                mask = literal;
            }
            duration = state->duration;
            elapsed = (u16)elapsed;
            reverseDuration = duration;
            if (elapsed >= duration) {
                u32 next;
                register u32 flags asm("r1");
                state->elapsed = 0;
                flags = state->flags;
                if (flags & 4) {
                    next = state->index;
                    if (next == 0) {
                        if (flags & 8) {
                            state->elapsed = reverseDuration;
                            goto completed_reverse;
                        }
                        next = state->frameCount;
                        state->index = next - 1;
                    completed_reverse:
                        done = 1;
                    } else {
                        state->index = next - 1;
                        done = 0;
                    }
                } else {
                    u32 count;
                    next = state->index + 1;
                    state->index = next;
                    count = state->frameCount;
                    if ((next & mask) >= count) {
                        u32 clamp = (u8)(flags & 8);
                        if (clamp) {
                            state->elapsed = duration;
                            clamp = count - 1;
                        }
                        state->index = clamp;
                        done = 1;
                    } else
                        done = 0;
                }
                command = &state->commands[state->index];
                {
                    register u32 data asm("r1") = *command;
                    state->frameWeight = data & 15;
                }
                {
                    s32 weight = state->frameWeight;
                    s32 speed = state->speed;
                    s32 delay = (speed * weight) >> 6;
                    state->duration = delay;
                    if (!(u8)delay)
                        state->duration = 1;
                }
            } else
                done = 0;
        }
        if (done && track->sequenceEvent) {
            track->flags &= ~8;
            if (track->sequenceEventValue)
                Script_Call(track->sequenceEventValue, 0);
        }
    }
}
