#include "os.h"

int blink(int);
int update_dial(int);
int collect_delta(int);
int check_switches(int);

FIL File;  						/* FAT File */

int position = 0;

int main(void) {
    os_init();

    os_add_task( blink,            30, 1);
    os_add_task( collect_delta,   500, 1);
    os_add_task( check_switches,  100, 1);

    sei();
	
	display_string("Hello, World!\n");
	
    for(;;){}
}

int collect_delta(int state) {
	position += os_enc_delta();
	return state;
}

int check_switches(int state) {
	if (get_switch_long(_BV(SWC))) {
		f_mount(&FatFs, "", 0);
		if (f_open(&File, "positions.txt", FA_WRITE | FA_OPEN_ALWAYS) == FR_OK) {
			f_lseek(&File, f_size(&File));
			f_printf(&File, "Encoder position is: %d \r\n", position);
			f_close(&File);
			display_string("Wrote position\n");
		} else {
			display_string("Can't write file! \n");
		}
	}

	if (get_switch_short(_BV(SWC))) display_string("Button Pressed\n");

	if (get_switch_long(_BV(OS_CD))) display_string("Detected SD card.\n");

	return state;
}

int blink(int state) {
	static int light = 0;
	uint8_t level;

	if (light < -120) {
		state = 1;
	} else if (light > 254) {
		state = -20;
	}

	/* Compensate somewhat for nonlinear LED
       output and eye sensitivity:
    */
	if (state > 0) {
		if (light > 40) {
			state = 2;
		}
		if (light > 100) {
			state = 5;
		}
	} else {
		if (light < 180) {
			state = -10;
		}
		if (light < 30) {
			state = -5;
		}
	}
	light += state;

	if (light < 0) {
		level = 0;
	} else if (light > 255) {
		level = 255;
	} else {
		level = light;
	}

	os_led_brightness(level);
	return state;
}
