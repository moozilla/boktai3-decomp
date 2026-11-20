; Full-screen pages use the same signed BG tile addressing as the rooms.
; Bank switches do not disturb player, actor or inventory WRAM.
static_page:
    push hl
    call lcd_off
    ld a,screen_bank
    ld [0x2000],a
    pop hl
    ld de,0x8800
    ld bc,0x1400
    call copy
    xor a
    ldh [0x42],a
    ldh [0x43],a
    ld a,0x81
    ldh [0x40],a
    ret

title_screen:
    ld a,1
    ld [mode],a
    ld hl,0x4000
    jp static_page
open_meter:
    ld a,2
    ld [mode],a
    ld hl,0x5400
    jp static_page
open_link:
    ld a,3
    ld [mode],a
    ld hl,0x6800
    call static_page
    call printer_init
    ret
restore_field:
    call lcd_off
    xor a
    ld [mode],a
    ld [menu],a
    call room_vram
    ld a,1
    ld [camera_snap],a
    jp show_field

static_tick:
    cp 3
    jp z,link_frame
    call input
    ld a,[mode]
    cp 1
    jr nz,.meter
    ld a,[pressed]
    and 9
    jr z,.idle
    xor a
    ld [mode],a
    call load_room
    jp loop
.idle:
    call vblank
    jp loop
.meter:
    ld a,[pressed]
    bit 1,a
    jr nz,.close
    bit 3,a
    jr nz,.link
    bit 0,a
    jr nz,.choose
    ld a,[pressed_dir]
    bit 1,a
    jr z,.right
    ld hl,shadow_choice
    ld a,[hl]
    and a
    jr z,.right
    dec [hl]
.right:
    ld a,[pressed_dir]
    bit 0,a
    jr z,.cursor
    ld hl,shadow_choice
    ld a,[hl]
    cp 2
    jr nc,.cursor
    inc [hl]
.cursor:
    call vblank
    ld a,126
    ld [0x9983],a
    ld [0x9989],a
    ld [0x998f],a
    ld hl,0x9983
    ld de,6
    ld a,[shadow_choice]
    and a
    jr z,.mark
    add hl,de
    dec a
    jr z,.mark
    add hl,de
.mark:
    ld [hl],125
    jp loop
.choose:
    ld a,[shadow_choice]
    cp 2
    jr nz,.read
    inc a
.read:
    ld [sun_level],a
    ld a,1
    ld [sun_source],a
.close:
    call restore_field
    jp loop
.link:
    call open_link
    jp loop

; The Camera is the link clock master. Poll at serial-byte cadence even
; during active scan; the CPU never needs to store a full photograph.
link_frame:
.sync:
    call serial_poll
    ldh a,[0x44]
    cp 144
    jr nc,.sync
.active:
    call serial_poll
    ldh a,[0x44]
    cp 144
    jr c,.active
    call input
    ld a,[pressed]
    bit 1,a
    jr nz,.back
    bit 0,a
    jr z,.show
    ld a,[photo_ready]
    and a
    jr z,.show
    ld a,[photo_level]
    ld [sun_level],a
    ld a,2
    ld [sun_source],a
    xor a
    ldh [2],a
    call restore_field
    jp loop
.back:
    xor a
    ldh [2],a
    call open_meter
    jp loop
.show:
    ld a,[photo_ready]
    and a
    jp z,loop
    ld a,[photo_level]
    inc a
    ld c,a
    ld b,4
    ld d,127
    ld e,126
    ld hl,0x99c1
    call meter
    jp loop
serial_poll:
    ldh a,[2]
    bit 7,a
    ret nz
    ldh a,[1]
    call printer_byte
    ldh [1],a
    ld a,128
    ldh [2],a
    ret
