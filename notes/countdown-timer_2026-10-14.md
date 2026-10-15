# Singleton countdown timer

The0805751C–08057678 cohort contains eight functions /348 bytes. One new
80-byte initializer adds0.0033 percentage points without hints. The remaining
seven functions already matched and now share CountdownTimer's typed view.

The0200049C singleton is a0x20-byte list8 task: halfword remaining ticks at18,
unknown/lock halfword1A and integer completion script ID1C. Initialization reads
t ticks(default1800), p completion ID(default0), and i(default0). Unless i is
nonzero it passes the address of the remaining-ticks halfword to081783C0.
That HUD consumer stores the pointer at02000214+B04 and enables its display;
08178404 clears the pointer and hides its three animation entries. These static
consumers support a displayed countdown, without identifying a particular event.

Each tick decrements the halfword, then on wrapped result zero calls a nonzero
completion ID with argument0 and stops. Initial zero wraps to65535 rather than
completing immediately. Stop detaches the HUD pointer, zeroes the current timer
and kills its task; destruction clears the singleton. Script creation returns an
existing instance, otherwise allocates and initializes one. Two unused incoming
arguments remain in the initializer/creator ABI because the original creator
forwards them. The script extension command adds t(default0) only when lock1A
is zero and remaining is nonzero, with halfword wrapping preserved. This cohort
does not write the lock field, so its broader meaning remains tentative.

All eight complete reviewed extents match. No new execution coverage is claimed.
