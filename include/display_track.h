#ifndef GUARD_DISPLAY_TRACK_H
#define GUARD_DISPLAY_TRACK_H
#include "contact_swarm.h"
/* Provisional: script-controlled sprite movement, palette and display tracks. */
struct DisplayTrackGraphics {
    u8 unknown0[6];
    u16 defaultPalette;
    u8 unknown8[4];
    const void *palette;
    u8 unknown10[0xC];
};
struct DisplayTrack {
    u16 tag;
    u8 flags, mirror, runDuringPause;
    u8 unknown5[3];
    u16 blendedPalette[16];
    struct ContactSwarmSprite sprite;
    struct DisplayTrackGraphics graphics;
    struct ContactPosition origin, target;
    u16 moveDuration, moveFrame, paletteBase, paletteStart, paletteEnd, paletteFrame,
        paletteDuration, unknown8E;
    u16 scaleStart, scaleEnd, scaleFrame, scaleDuration;
    u8 sequenceId, sequenceRow, sequenceEvent, unknown9B;
    struct DisplaySequenceState sequenceState;
    const void *sequenceResource;
    u32 sequenceEventValue;
    void *ownedResource;
};
struct DisplayTrackManager {
    u8 task[0x18];
    u8 capacity, count, unknown1A[2];
    struct DisplayTrack entries[1];
};
extern struct DisplayTrackManager *gUnk_02000494;
struct DisplayTrack *sub_08054B80(void);
#endif
