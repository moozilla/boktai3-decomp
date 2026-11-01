# Tentacle segment animation status

Tags: #monsters #matching #depin

The segment status test in all three directional-animation callbacks accepts
an eight-bit mask. That form matches the register-pinned test, including its
load and comparison order, and reads the ordinary status byte directly.
The shared segment state carries the display sequence at offset 0xF0.
