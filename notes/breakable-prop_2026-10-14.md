# Breakable prop, 080515C0–08051A3C

All eight functions match across1,148 bytes. New EmitBreakParticles515DC(288)
and InitializeFromScript51858(416) add0.0291 percentage points (704 bytes), with
four and six minimized allocation hints/barriers respectively.

The creator allocates0xD0 bytes on task list8. Shared breakable_prop.h records
sprite18 (world position34), graphics44, collider60, terrain registrationB4,
heightC5, cellC6, destroyRequestedC8 and script IDCC. Contact callback515C0 uses
three arguments: contact, unused scalar, owner. Contact flag200 at offset34
requests destruction. This flag offset differs from the collider reaction
flags at38; the callback has a separate observed contact view.

Tick hides the sprite, emits six particles, consumes an optional script ID
before Script_Call, and marks the task for deletion. Cleanup removes sprite,
collider and terrain registration. Its original nonvoid fallthrough is retained.

Initializer p gives XYZ; X/Z are centered on their256-unit cells. Missing p
starts at zero. The map cell selects terrain properties through0821D64C or the
base-map fallback. Surface1 subtracts the low Z byte from the cell height;
surface2 subtracts low X. The resulting Y is written into the packed position,
and height registration uses (Y>>8)+1, flags103 and value255. Sprite resource t
defaults9D41; i sets the frame halfword at sprite+10; r toggles flag4. Collider
bounds are128 on each axis, offsetY128, flags4001 and value20. Script R is an
optional resource ID, never a host pointer. The creator forwards a third
initializer argument that is unused in the recovered body.

Particles copy the complete source position packet, jitter X/Z by[-150,149],
add[0,299] to Y, and call AnimatedParticles_Create(1,...). RNG order is XYZ per
particle. Their40-byte configuration leaves halfword4 untouched, matching the
original; the particle consumer154EC does not read it. Whole-word masked
position updates preserve copied padding. No uninitalized packet is read by
these new particle calculations. The collider helper retains the original
packed-vector padding convention, as in other collider initializers.
