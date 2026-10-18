#ifndef FOUNDRY_MODIFIER_ACTION_H
#define FOUNDRY_MODIFIER_ACTION_H
struct ModifierActionOwner {
    u8 unknown0[0x18];
    u8 stats[0x58];
    s16 available;
};
struct ModifierAction {
    u8 unknown0[6];
    u16 duration, cost;
    u8 unknownA[2];
    u16 interval;
    u8 unknownE[3];
    u8 status;
    u16 remaining, cooldown;
    u8 unknown16[2];
    struct ModifierActionOwner *owner;
};
#endif
