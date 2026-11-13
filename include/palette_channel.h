#ifndef GUARD_PALETTE_CHANNEL_H
#define GUARD_PALETTE_CHANNEL_H

/* Keep the channel in its packed RGB555 position for fixed-point scaling. */
#define PALETTE_EXTRACT_CHANNEL(out, color, mask) \
    do { \
        (out) = (color) & (mask); \
    } while (0)

#endif
