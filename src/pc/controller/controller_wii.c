#ifdef TARGET_GX
#ifdef __wii__

#define Mtx __Mtx
#define Vtx __Vtx
#define guPerspectiveF __guPerspectiveF
#define guPerspective __guPerspective
#define guOrtho __guOrtho
#define guOrthoF __guOrthoF
#include <wiiuse/wpad.h>
#undef Mtx
#undef Vtx
#undef guPerspectiveF
#undef guPerspective
#undef guOrtho
#undef guOrthoF

#include <ultra64.h>

#include "controller_api.h"

#include "../configfile.h"

static int button_mapping[9][2];

static void set_button_mapping(int index, int mask_n64, int mask_wii)
{
    button_mapping[index][0] = mask_wii;
    button_mapping[index][1] = mask_n64;
}

static uint32_t controller_wii_get_held(void)
{
    uint32_t res = 0;

    WPAD_ScanPads();
    uint32_t kDown = WPAD_ButtonsHeld(0);

    for (size_t i = 0; i < sizeof(button_mapping) / sizeof(button_mapping[0]); i++)
    {
        if (button_mapping[i][0] & kDown) {
            res |= button_mapping[i][1];
        }
    }
    return res;
}

static void controller_wii_init(void) {
    WPAD_Init();

    uint8_t i;
    set_button_mapping(i++, A_BUTTON,     configKeyA); // n64 button => configured button
    set_button_mapping(i++, B_BUTTON,     configKeyB);
    set_button_mapping(i++, START_BUTTON, configKeyStart);
    set_button_mapping(i++, R_TRIG,       configKeyR);
    set_button_mapping(i++, Z_TRIG,       configKeyZ);
    set_button_mapping(i++, U_CBUTTONS,   configKeyCUp);
    set_button_mapping(i++, D_CBUTTONS,   configKeyCDown);
    set_button_mapping(i++, L_CBUTTONS,   configKeyCLeft);
    set_button_mapping(i++, R_CBUTTONS,   configKeyCRight);
}

static void controller_wii_read(OSContPad *pad)
{
    pad->button = controller_wii_get_held();

    struct expansion_t data;
    WPAD_Expansion(WPAD_CHAN_0, &data);

    if (data.type == WPAD_EXP_NUNCHUK)
    {
        pad->stick_x = data.nunchuk.js.pos.x - data.nunchuk.js.center.x;
        pad->stick_y = data.nunchuk.js.pos.y - data.nunchuk.js.center.y;
    }
    else if (data.type == WPAD_EXP_CLASSIC)
    {
        pad->stick_x = 2.0f * (data.classic.ljs.pos.x - data.classic.ljs.center.x);
        pad->stick_y = 2.0f * (data.classic.ljs.pos.y - data.classic.ljs.center.y);

        bool inverted_look = false;
        s8 deadzone = 10;

        s8 ssx = data.classic.rjs.pos.x - data.classic.rjs.center.x;
        s8 ssy = data.classic.rjs.pos.y - data.classic.rjs.center.y;

        if (ssx > deadzone)
            pad->button |= inverted_look ? L_CBUTTONS : R_CBUTTONS;
        if (ssx < -deadzone)
            pad->button |= inverted_look ? R_CBUTTONS : L_CBUTTONS;
        if (ssy > deadzone)
            pad->button |= inverted_look ? D_CBUTTONS : U_CBUTTONS;
        if (ssy < -deadzone)
            pad->button |= inverted_look ? U_CBUTTONS : D_CBUTTONS;
    }
}

struct ControllerAPI controller_wii = {
    controller_wii_init,
    controller_wii_read
};

#endif
#endif
