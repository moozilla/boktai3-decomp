// MATCHING_CONCESSION: register-pinning
#include "display_track.h"
#include "display_sprite_playback.h"

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
        u32 done = PlayerCue_StepSequenceEnded(&track->sprite, &track->sequenceState);
        if (done && track->sequenceEvent) {
            track->flags &= ~8;
            if (track->sequenceEventValue)
                Script_Call(track->sequenceEventValue, 0);
        }
    }
}
