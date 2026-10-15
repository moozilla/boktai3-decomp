# Directional force controller, 080510B8–080515C0

All 12 functions match across 1,288 bytes. New Tick51224(364) and
InitializeFromScript513B8(444) add 0.0335 percentage points (808 bytes), with
locally minimized allocation concessions (five and six hints respectively).

The task allocated by51574 is0x30 bytes on list9; singleton02000114. Mode0
stops retargeting and clears target magnitude, target angle and delay while
preserving current values for smooth decay. Mode1 randomizes magnitude,
eight-bit direction and delay. Mode2 uses a fixed direction and randomizes
magnitude and delay. Script m selects mode (default1), H sets global0200052C
(default0), p gives base/range magnitude (default3/3), t gives base/range
delay (default120/120), and d gives mode2 direction (default0).

Tick smooths magnitude as (15*current+target)>>4. Angle uses twelve-bit wrapped
space, moving one sixteenth of the shortest arc toward targetAngle<<4. Published
magnitude02000550 and angle0200053C are the smoothed values shifted right4.
Threshold0200052C is consumed alongside them by the existing guarded
TetherActor_AdjustSlopeMovement draft4945C. A wind-like gameplay interpretation
is plausible but unconfirmed; DirectionalForce remains provisional.

RNG order matters: mode1 initialization draws angle first, then target
magnitude, then delay; mode1 retargeting draws magnitude, angle, delay. Mode2
initialization does not assign current magnitude. The initializer retains the
original nonvoid fallthrough, including its unspecified return value; the
creator still performs its original negative-return check. Destroy clears all
three published globals and the singleton. SetThreshold11EC retains its
pre-existing old_agbcc selection and u16 argument contract.
