#include "engine/shared/display_track.h"

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
            u16 *frame;
            u16 *defaultPalette;
            u16 *initial;
            u16 *period;
            u16 *out;
            initial = track->blendedPalette;
            defaultPalette = &track->graphics.defaultPalette;
            frame = &track->paletteFrame;
            period = duration;
            out = initial;
            for (i = 0; i < 16; i++) {
                u32 from = *start;
                u32 redStart = 31;
                u32 greenStart, blueStart, redEnd, greenEnd, blueEnd;
                u32 to;
                redStart &= from;
                greenStart = (u16)from >> 5 & 31;
                blueStart = (u16)from >> 10 & 31;
                to = *end;
                redEnd = 31;
                redEnd &= to;
                greenEnd = (u16)to >> 5 & 31;
                blueEnd = (u16)to >> 10 & 31;
                {
                    s32 red = Div(redStart * left + redEnd * *frame, *period);
                    s32 green = Div(greenStart * left + greenEnd * *frame, *period);
                    s32 blue = Div(blueStart * left + blueEnd * *frame, *period);
                    *out = (blue << 10) | (green << 5) | red;
                }
                start++;
                end++;
                out++;
            }
            track->graphics.palette = initial;
            *defaultPalette = 600 + track->tag;
        }
    }
    if (track->flags & 4) {
        if (++track->scaleFrame >= track->scaleDuration) {
            u32 end = track->scaleEnd;
            track->sprite.unknown8[0] = end;
            track->sprite.unknown8[1] = end;
            if ((s8)track->sprite.unknown8[0] == 64)
                track->sprite.flags &= ~2;
            track->flags &= ~4;
        } else {
            u32 scale;
            track->sprite.flags |= 2;
            scale = Div(track->scaleStart * (track->scaleDuration - track->scaleFrame) +
                            track->scaleEnd * track->scaleFrame,
                        track->scaleDuration);
            track->sprite.unknown8[0] = scale;
            track->sprite.unknown8[1] = scale;
        }
    }
    if (track->flags & 8) {
        u32 done = DisplayTrack_StepSequence(track, &track->sprite, &track->sequenceState);
        if (done && track->sequenceEvent) {
            track->flags &= ~8;
            if (track->sequenceEventValue)
                Script_Call(track->sequenceEventValue, 0);
        }
    }
}
