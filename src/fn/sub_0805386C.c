#include "interaction_marker.h"
void InteractionMarker_TickIdle(struct InteractionMarker *marker)
{
    u32 nearby;
    if ((!(gEngineFlags & 0x200)) && (marker->inputCooldown <= 9)) {
        marker->inputCooldown++;
    }
    nearby = InteractionMarker_IsPlayerNearby(marker);
    if (nearby) {
        if (marker->nearby != 1) {
            Sound_PlaySong(0x198);
            marker->nearby = 1;
        }
        if (marker->activationFrame <= 7) {
            marker->activationFrame++;
        }
        if (marker->height <= 22) {
            marker->height += 2;
        } else {
            marker->height = 24;
        }
        {
            u32 button = 1;
            if (gUnk_02000560 & button) {
                u32 pressed = gPadState[1];
                u32 edge = button;
                do {
                    edge &= pressed;
                } while (0);
                if (edge) {
                    struct InteractionMarkerPlayer *player = gPlayers[0];
                    u32 blocked = 0x100000;
                    if (!(player->flags & blocked) && marker->inputCooldown > 9) {
                        marker->activationFrame = 8;
                        ScriptUtility_SetControlFlag(1);
                        PlayerCue_WalkTo(gPlayers[0], &marker->position, 0, 4);
                        InteractionMarker_SetState(marker, InteractionMarker_TickInteraction);
                    }
                }
            }
        }
    } else {
        if (marker->nearby != 0) {
            Sound_PlaySong(0x199);
            marker->nearby = nearby;
        }
        if (marker->activationFrame != 0) {
            marker->activationFrame--;
        }
        if (marker->height > 2) {
            marker->height -= 2;
        } else {
            marker->height = nearby;
        }
    }
    if (marker->height == 0) {
        marker->primary.sprite.flags |= 1;
    } else {
        marker->primary.sprite.flags &= ~1;
        marker->primary.sprite.position.y = marker->position.y + marker->height;
    }
    if (marker->activationFrame > 7) {
        AnimData_SelectPaletteBank(&marker->primary.graphics, marker->paletteBase + 2);
    } else if (marker->activationFrame != 0) {
        AnimData_SelectPaletteBank(&marker->primary.graphics, marker->paletteBase + 1);
    } else {
        AnimData_SelectPaletteBank(&marker->primary.graphics, marker->paletteBase);
    }
}
