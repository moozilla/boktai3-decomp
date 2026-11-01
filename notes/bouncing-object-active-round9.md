# BouncingObject_TickActive

The active callback clears sprite flag bit zero. Its distance check uses a
stack displacement, while the bounce, movement and collider updates use the
sprite position. It retains the starting cell for comparison with the proposed
cell and preserves sequence timing when switching the blink animation.

The guarded C scores 74.3% over the reviewed 1,584-byte extent, up from 45.1%.
The candidate emits 1,592 bytes. Seventy realigned instructions still differ
after ignoring register choices. The original independently keeps the input
object, sprite position and animation-delay address live through these phases.
