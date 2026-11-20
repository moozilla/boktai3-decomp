; Delta-timed register score. All four voices share one frame clock.
; Each event is delay, channel mask, then three bytes per changed voice.
music_init:
    xor a
    ldh [0x1a],a
    ld hl,wave_pattern
    ld de,0xff30
    ld bc,16
    call copy
    ; Force the first tick to reload the beginning of the switch window.
    ld a,0x40
    ld [music_hi],a
    xor a
    ld [music_lo],a
    ld [music_wait],a
    ret

music_tick:
    ld hl,sfx_pulse
    ld a,[hl]
    and a
    jr z,.noise
    dec [hl]
.noise:
    ld hl,sfx_noise
    ld a,[hl]
    and a
    jr z,.clock
    dec [hl]
.clock:
    ld hl,music_wait
    ld a,[hl]
    and a
    jr z,.read
    dec [hl]
    ret nz
.read:
    ld a,music_bank
    ld [0x2000],a
    ld a,[music_hi]
    ld h,a
    ld a,[music_lo]
    ld l,a
    ld a,[hl+]
    and a
    jr nz,.event
    ld hl,music_loop
    ld a,[hl+]
.event:
    ld [music_wait],a
    ld b,[hl]
    inc hl
    bit 0,b
    jr z,.square2
    call music_params
    ld a,c
    and 0xc0
    ldh [0x11],a
    ld a,e
    ldh [0x12],a
    ld a,d
    ldh [0x13],a
    ld a,c
    and 7
    or 0x80
    ldh [0x14],a
.square2:
    bit 1,b
    jr z,.wave
    call music_params
    ld a,[sfx_pulse]
    and a
    jr nz,.wave
    ld a,c
    and 0xc0
    ldh [0x16],a
    ld a,e
    ldh [0x17],a
    ld a,d
    ldh [0x18],a
    ld a,c
    and 7
    or 0x80
    ldh [0x19],a
.wave:
    bit 2,b
    jr z,.noise_voice
    call music_params
    xor a
    ldh [0x1a],a
    ld a,0x80
    ldh [0x1a],a
    ld a,e
    ldh [0x1c],a
    ld a,d
    ldh [0x1d],a
    ld a,c
    or 0x80
    ldh [0x1e],a
.noise_voice:
    bit 3,b
    jr z,.done
    call music_params
    ld a,[sfx_noise]
    and a
    jr nz,.done
    ld a,e
    ldh [0x21],a
    ld a,d
    ldh [0x22],a
    ld a,c
    ldh [0x23],a
.done:
    ld a,l
    ld [music_lo],a
    ld a,h
    ld [music_hi],a
    ld a,[bank]
    ld [0x2000],a
    ret
music_params:
    ld d,[hl]
    inc hl
    ld c,[hl]
    inc hl
    ld e,[hl]
    inc hl
    ret
