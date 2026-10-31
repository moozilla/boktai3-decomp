# Dog_Init

Tags: #matching #dog #compiler #round8

All 4,304 bytes at 080E5744 match with stock agbcc. The two palette paths each set the primary speed to 0x40000, invalidate the cached cell, and copy the primary speed to the current speed. Only the alternate path also sets the alternate speed to 0x38400.

Keeping the cache and current-speed assignments in both paths recovers the independent field address calculations. At that point the only differences are nine bytes encoding the registers for the context and the primary speed address. Capturing the primary field in the alternate path gives its address enough uses to be allocated first. It takes r2, leaving r3 for the context. The shared monster initialization operations supply the rest of the routine.
