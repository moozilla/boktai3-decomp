#ifndef GUARD_INTERACTION_MARKER_H
#define GUARD_INTERACTION_MARKER_H
#include "contact_swarm.h"
struct InteractionMarker {
    u8 task[0x18];
    u16 id;
    u8 nearby, bodyEnabled;
    u8 unknown1C[4], body[0x48];
    struct ContactSwarmDrawPacket primary;
    struct ContactPosition position;
    struct ContactSwarmDrawPacket secondary;
    u16 baseTile, activationFrame, height, inputCooldown;
    u16 scriptArgs[2];
    u32 scriptId;
    u16 stateFrame, unknown112;
    void (*update)(struct InteractionMarker *);
};
struct InteractionMarkerPlayer {
    u8 unknown0[0x20];
    u32 flags;
    u8 unknown24[0xC];
    struct ContactPosition position;
    u8 unknown38[0x41F];
    u8 action;
    u8 unknown458[0xC];
    u8 busy;
};
extern struct InteractionMarkerPlayer *gUnk_02000580[4];
extern u32 gUnk_02000560;
extern u16 gUnk_03005260[];
u32 sub_080537D0(struct InteractionMarker *);
void sub_08053854(struct InteractionMarker *, void (*)(struct InteractionMarker *));
void sub_0805386C(struct InteractionMarker *);
void sub_080539F4(struct InteractionMarker *);
void sub_08053AAC(struct InteractionMarker *);
void sub_08056C6C(u32);
void sub_0815B0A8(struct InteractionMarkerPlayer *, struct ContactPosition *, u32, u32);
u32 sub_08053AB0(struct InteractionMarker *);
u32 sub_08053AC4(struct InteractionMarker *);
s32 sub_08053AEC(struct InteractionMarker *, u32, u32);
#endif
