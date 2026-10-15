# Moving tile particles (tentative identity)

`08069FA4–0806A60C` owns the `02000168` pool: task size `0xCE4`, 48 entries
at `0x18`, stride `0x44`; sequence resource `0xCD8`, graphics `0xCDC`, active
count `0xCE0`. Each entry contains a 16-byte sequence, 40-byte tile sprite,
position velocity at `0x38`, and active byte `0x40`.

The updater adds all three velocity coordinates, decodes tile and flips,
steps forward/reverse sequence timing, and removes completed particles from
the active count while hiding their sprites. Reaching the first/last frame
counts as completion for both looping and held-end sequences. The initializer
finds script resource `(0x922E,0xD1B8)` and graphics `0x1C1A`, initializes all
48 sprites with offset `(-8,-10)` and render mode2, and hides them.

The creator `6A160` is reviewed as704 bytes, ending at `6A420`. It remains
an ignored draft with a seven-register-byte residue in kind-selection reloads;
its background permuter starts at score35. Creation accepts an optional
per-axis spread and applies random offset minus signed half-spread. Wave
velocity uses truncation toward zero. The random vertical range writes
velocity Y, not position Y. Count uses its low eight bits; a requested count
zero wraps on first decrement and continues until available slots run out.
The function returns zero only on pool-creation failure, otherwise one even
when no inactive entry exists.

`6A420` is a separate256-byte script wrapper hidden in the old disassembly's
creator extent. It has its own prologue, stack frame, return, and literal pool;
both build and progress metadata now record the boundary. `n` defaults0;
zero count or absent `p` returns before calling the creator. `p` supplies
three required coordinate values. Optional `o` supplies three spread values,
otherwise an eight-byte CpuSet clears the spread. Keywords `d/s/k/r` default
to0/0/1/2. The wrapper always passes vertical range zero.

The wrapper's union objects give both packed-field write and coordinate read
views. An empty memory-output barrier gives the zero temporary its original
first stack slot; the temporary is explicitly assigned before CpuSet and no
uninitialized value is consumed. Remaining hints are defined empty operands
or register bindings. The updater retains17 minimized hints; the hidden
wrapper retains5. Neither binds r7 or supplies instruction bodies.

Exact new progress so far: updater404 bytes, initializer148 ordinary bytes,
script wrapper256 bytes:808 bytes, about0.0335 percentage points. Existing
128 lifecycle bytes have typed contracts without new credit. Full ROM batch
validation belongs to the integrator.

The next manager begins at `6A60C`, not `6A694`: that helper throttles collider
refreshes for the separate24-entry `02000178` sprite controller. Its shared
view is `sprite_effect_controller.h`, also used by the adjoining worker.
