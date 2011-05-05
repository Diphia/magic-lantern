/** \file
 * Magic Lantern GUI main task.
 *
 * Overrides the DryOS gui_main_task() to be able to re-map events.
 */
/*
 * Copyright (C) 2009 Trammell Hudson <hudson+ml@osresearch.net>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#include "dryos.h"
#include "property.h"
#include "bmp.h"
#include "config.h"
#include "consts-60d.109.h"
#include "lens.h"

CONFIG_INT("previous.photo.mode", previous_photo_mode, SHOOTMODE_M);

int lv_disp_mode;
PROP_HANDLER(PROP_HOUTPUT_TYPE)
{
	lv_disp_mode = buf[1];
	return prop_cleanup(token, property);
}

static PROP_INT(PROP_GUI_STATE, gui_state);
static PROP_INT(PROP_DISPSENSOR_CTRL, display_sensor_neg);
static PROP_INT(PROP_SHOOTING_MODE, shooting_mode);
static PROP_INT(PROP_LV_DISPSIZE, lv_dispsize);
static PROP_INT(PROP_MVR_REC_START, recording);

//~ int button_center_lvafframe = BGMT_PRESS_SET;

// halfshutter press is easier to detect from GUI events (PROP_HALFSHUTTER works only in LV mode)
int halfshutter_pressed = 0;
int get_halfshutter_pressed() 
{ 
	return halfshutter_pressed; 
}

int zoom_in_pressed = 0;
int zoom_out_pressed = 0;
//~ int set_pressed = 0;
int get_zoom_in_pressed() { return zoom_in_pressed; }
int get_zoom_out_pressed() { return zoom_out_pressed; }
//~ int get_set_pressed() { return set_pressed; }

struct semaphore * gui_sem;


struct gui_main_struct {
	void *			obj;		// off_0x00;
	uint32_t		counter;	// off_0x04;
	uint32_t		off_0x08;
	uint32_t		counter_60d;    //off_0x0c;
	uint32_t		off_0x10;
	uint32_t		off_0x14;
	uint32_t		off_0x18;
	uint32_t		off_0x1c;
	uint32_t		off_0x20;
	uint32_t		off_0x24;
	uint32_t		off_0x28;
	uint32_t		off_0x2c;
	struct msg_queue *	msg_queue_60d;	// off_0x30;
	struct msg_queue *	msg_queue;	// off_0x34;
	struct msg_queue *	msg_queue_550d;	// off_0x38;
	uint32_t		off_0x3c;
};

extern struct gui_main_struct gui_main_struct;

struct gui_timer_struct
{
	void *			obj;	// off_0x00
};

extern struct gui_timer_struct gui_timer_struct;

extern void* gui_main_task_functbl;

CONFIG_INT("set.on.halfshutter", set_on_halfshutter, 1);

// return 0 if you want to block this event
static int handle_buttons(struct event * event)
{
	static int kev = 0;

	// volume adjust (FLASH + UP/DOWN) and ISO adjust (FLASH + LEFT/RIGHT)
	/*
	if (shooting_mode == SHOOTMODE_MOVIE && gui_state == GUISTATE_IDLE && FLASH_BTN_MOVIE_MODE)
	{
		if (event->type == 0 && event->param == BGMT_PRESS_UP)
		{
			volume_up();
			falsecolor_cancel();
			return 0;
		}
		if (event->type == 0 && event->param == BGMT_PRESS_DOWN)
		{
			volume_down();
			falsecolor_cancel();
			return 0;
		}
		if (event->type == 0 && event->param == BGMT_PRESS_LEFT)
		{
			iso_toggle(-1);
			falsecolor_cancel();
			return 0;
		}
		if (event->type == 0 && event->param == BGMT_PRESS_RIGHT)
		{
			iso_toggle(1);
			falsecolor_cancel();
			return 0;
		}
	}*/

	// event 0 is button press maybe?
	if(event->type == 0 && ( gui_state == GUISTATE_IDLE || MENU_MODE))
	{
		if (event->param == BGMT_TRASH || event->param == BGMT_UNLOCK)
		{
			if (!gui_menu_shown()) 
				give_semaphore( gui_sem );
			else
				gui_stop_menu();
			return 0;
		}
	}

	if (event->type == 0 && event->param != 0x5a)
	{
		clearscreen_wakeup();
	}
	
	if (get_draw_event())
	{
		if (event->type == 0)
		{
			kev++;
			bmp_printf(FONT_SMALL, 0, 400, "Ev%d[%d]: p=%8x *o=%8x/%8x/%8x a=%8x", 
				kev,
				event->type, 
				event->param, 
				event->obj ? *(uint32_t*)(event->obj) : 0,
				event->obj ? *(uint32_t*)(event->obj + 4) : 0,
				event->obj ? *(uint32_t*)(event->obj + 8) : 0,
				event->arg);
			/*console_printf("Ev%d[%d]: p=%8x *o=%8x/%8x/%8x a=%8x\ns", 
				kev,
				event->type, 
				event->param, 
				event->obj ? *(uint32_t*)(event->obj) : 0,
				event->obj ? *(uint32_t*)(event->obj + 4) : 0,
				event->obj ? *(uint32_t*)(event->obj + 8) : 0,
				event->arg);*/
			//msleep(250);
		}
#if 0
		// simple debugging of memory location - focus data in memory maybe?
		static int ii = 0;
		ii++;
		bmp_printf(FONT_SMALL, 0, ii*15, "%08x %08x %08x %08x %08x", *(uint32_t*)(0x54B4), *(uint32_t*)(0x54B4+0xC), *(uint32_t*)(0x54B4+0x10), *(uint32_t*)(0x54B4+0x18), *(uint32_t*)(0x54B4+0x22));
		if (ii>30) ii = 0;
#endif

	}
	
	if (gui_menu_shown() && event->type == 0) // some buttons hard to detect from main menu loop
	{
		if (lv_drawn() && event->param == BGMT_UNPRESS_HALFSHUTTER || event->param == BGMT_UNPRESS_ZOOMOUT_MAYBE) // zoom out unpress, shared with halfshutter
		{
			gui_hide_menu( 2 );
			lens_focus_stop();
			return 0;
		}
		if (lv_drawn() && (event->param == BGMT_PRESS_HALFSHUTTER || event->param == BGMT_PRESS_ZOOMOUT_MAYBE)) // zoom out press, shared with halfshutter
		{
			gui_hide_menu( 50 );
			lens_focus_start( get_focus_dir() );
			return 0;
		}
	}
	if (gui_menu_shown())
	{
		if (event->type == 0 && event->param == 0x5a) return 0;
	}
	
	/*
	if (get_lcd_sensor_shortcuts() && event->type == 0 && display_sensor_neg == 0 && DISPLAY_SENSOR_POWERED) // button presses while display sensor is covered
	{ // those are shortcut keys
		if (!gui_menu_shown())
		{
			if (event->param == BGMT_PRESS_UP)
			{
				adjust_backlight_level(1);
				return 0;
			}
			else if (event->param == BGMT_PRESS_DOWN)
			{
				adjust_backlight_level(-1);
				return 0;
			}
		}
		if (lv_drawn())
		{
			if (event->param == BGMT_PRESS_LEFT)
			{
				kelvin_toggle(-1);
				return 0;
			}
			else if (event->param == BGMT_PRESS_RIGHT)
			{
				kelvin_toggle(1);
				return 0;
			}
		}
	}*/

	if (event->type == 0)
	{
		if (is_follow_focus_active() && !is_manual_focus() && !gui_menu_shown() && lv_drawn() && gui_state == GUISTATE_IDLE)
		{
			switch(event->param)
			{
				case BGMT_PRESS_LEFT:
					lens_focus_start(1 * get_follow_focus_dir_h());
					return 0;
				case BGMT_PRESS_RIGHT:
					lens_focus_start(-1 * get_follow_focus_dir_h());
					return 0;
				case BGMT_PRESS_UP:
					lens_focus_start(5 * get_follow_focus_dir_v());
					return 0;
				case BGMT_PRESS_DOWN:
					lens_focus_start(-5 * get_follow_focus_dir_v());
					return 0;
				case BGMT_UNPRESS_UDLR:
					lens_focus_stop();
					return 1;
			}
		}
	}
	
	if (event->type == 0)
	{
		if (event->param == BGMT_PRESS_HALFSHUTTER) halfshutter_pressed = 1;
		if (event->param == BGMT_UNPRESS_HALFSHUTTER) halfshutter_pressed = 0;
	}
	
	// force a SET press in photo mode when you adjust the settings and press half-shutter
	/*
	if (set_on_halfshutter && event->type == 0 && event->param == BGMT_PRESS_HALFSHUTTER && gui_state == GUISTATE_PLAYMENU && !lv_drawn() && !gui_menu_shown())
	{
		fake_simple_button(BGMT_PRESS_SET);
		fake_simple_button(BGMT_UNPRESS_SET);
	}*/
	
	// for faster zoom in in Play mode
	if (event->type == 0)
	{
		if (event->param == BGMT_PRESS_ZOOMIN_MAYBE) {zoom_in_pressed = 1; zoom_out_pressed = 0; }
		if (event->param == BGMT_UNPRESS_ZOOMIN_MAYBE) {zoom_in_pressed = 0; zoom_out_pressed = 0; }
		if (event->param == BGMT_PRESS_ZOOMOUT_MAYBE) { zoom_out_pressed = 1; zoom_in_pressed = 0; }
		if (event->param == BGMT_UNPRESS_ZOOMOUT_MAYBE) { zoom_out_pressed = 0; zoom_in_pressed = 0; }
 	}

	// override DISP button in LiveView mode
	
	/*if (event->type == 0 && event->param == BGMT_DISP && lv_drawn() && zebra_should_run())
	{
		if (lv_disp_mode == 0)
			return toggle_disp_mode();
		else
			schedule_disp_mode_change();
	}*/
	
	// MENU while recording => force a redraw
	if (recording && event->type == 0 && event->param == BGMT_MENU)
	{
		lv_redraw();
	}
	
	// stop intervalometer with MENU or PLAY
	if (event->type == 0 && (event->param == BGMT_MENU || event->param == BGMT_PLAY))
		intervalometer_stop();
		
	
	// zoom overlay
	
	if (get_zoom_overlay_z() && recording && event->type == 0 && event->param == BGMT_UNPRESS_ZOOMIN_MAYBE)
	{
		zoom_overlay_toggle();
	}
	
	/*
	if (get_lcd_sensor_shortcuts() && get_zoom_overlay_z() && lv_dispsize == 1 && event->type == 0 && event->param == BGMT_PRESS_ZOOMIN_MAYBE && display_sensor_neg == 0 && DISPLAY_SENSOR_POWERED)
	{
		zoom_overlay_toggle();
		return 0;
	}*/
	
	/*
	if (recording && get_zoom_overlay_mode())
	{
		if (event->type == 0 && event->param == BGMT_PRESS_LEFT)
			move_lv_afframe(-200, 0);
		if (event->type == 0 && event->param == BGMT_PRESS_RIGHT)
			move_lv_afframe(200, 0);
		if (event->type == 0 && event->param == BGMT_PRESS_UP)
			move_lv_afframe(0, -200);
		if (event->type == 0 && event->param == BGMT_PRESS_DOWN)
			move_lv_afframe(0, 200);
	}*/


/*
	
	if (event->param == 0 && *(uint32_t*)(event->obj) == PROP_SHUTTER)
	{
		int value = *(int*)(event->obj + 4);
		bmp_printf(FONT_LARGE, 0, 0, "Tv %d", value);
		DEBUG("Tv %d", value);
	}
	if (event->param == 0 && *(uint32_t*)(event->obj) == PROP_APERTURE)
	{
		int value = *(int*)(event->obj + 4);
		bmp_printf(FONT_LARGE, 0, 0, "Av %d", value);
		DEBUG("Av %d", value);
		
		static int old = 0;
		
		if (old && lv_drawn())
		{
			if (display_sensor_neg == 0)
			{
				if (value != old)
				{
					int newiso = lens_info.raw_iso + value - old;
					if (newiso >= 72 && newiso <= 120)
					{
						lens_set_rawiso(newiso);
					}
					else return 0;
				}
			}
		}
		old = value; 

	}
	if (event->param == 5 && *(uint32_t*)(event->obj) == 0x80010001)
	{
		DEBUG("limit");
		bmp_printf(FONT_MED, 0, 0, "Limit %8x %8x %8x %8x", 
			*(uint32_t*)(event->obj + 4), 
			*(uint32_t*)(event->obj + 8), 
			event->param,
			event->arg);
	}
	
	*/
	
	// quick access to some menu items
	
	if (event->type == 0 && event->param == BGMT_Q && !gui_menu_shown())
	{
		if (CURRENT_DIALOG_MAYBE == DLG_ISO)
		{
			select_menu("Expo", 0);
			give_semaphore( gui_sem ); 
			return 0;
		}
		/*else if (CURRENT_DIALOG_MAYBE == DLG_WB)
		{
			select_menu("Expo", 1);
			give_semaphore( gui_sem ); 
			return 0;
		}*/
		else if (CURRENT_DIALOG_MAYBE == DLG_FOCUS_MODE)
		{
			select_menu("Shoot", 5);
			give_semaphore( gui_sem ); 
			return 0;
		}
		else if (CURRENT_DIALOG_MAYBE == DLG_DRIVE_MODE)
		{
			select_menu("Shoot", 3);
			give_semaphore( gui_sem ); 
			return 0;
		}
		/*else if (CURRENT_DIALOG_MAYBE == DLG_PICTURE_STYLE)
		{
			select_menu("Expo", 7);
			give_semaphore( gui_sem ); 
			return 0;
		}
		else if (CURRENT_DIALOG_MAYBE == DLG_FLASH_AE)
		{
			select_menu("Expo", 9);
			give_semaphore( gui_sem ); 
			return 0;
		}
		else if (CURRENT_DIALOG_MAYBE == DLG_PICQ)
		{
			select_menu("Debug", 2);
			give_semaphore( gui_sem ); 
			return 0;
		}*/
		else if (lv_dispsize > 1)
		{
			select_menu("LiveV", 8);
			give_semaphore( gui_sem ); 
			return 0;
		}
		
	}

	/*
	if (event->param == 0 && *(uint32_t*)(event->obj) == PROP_APERTURE)
	{
		int value = *(int*)(event->obj + 4);
		//~ bmp_printf(FONT_LARGE, 0, 0, "Av %d", value);
		//~ DEBUG("Av %d", value);
		
		static int old = 0;
		
		if (get_lcd_sensor_shortcuts() && get_dof_adjust() && old && lv_drawn())
		{
			if (display_sensor_neg == 0)
			{
				if (value != old)
				{
					int newiso = lens_info.raw_iso + value - old;
					if (newiso >= 72 && newiso <= 120)
					{
						lens_set_rawiso(newiso);
					}
					else return 0;
				}
			}
		}
		old = value; 
	}*/
	
	// movie mode shortcut
	if (event->type == 0 && event->param == BGMT_LV && (CURRENT_DIALOG_MAYBE == DLG_DRIVE_MODE || CURRENT_DIALOG_MAYBE == DLG_ISO || CURRENT_DIALOG_MAYBE == DLG_FOCUS_MODE || CURRENT_DIALOG_MAYBE == DLG_METERING))
	{
		if (shooting_mode != SHOOTMODE_MOVIE)
		{
			previous_photo_mode = shooting_mode;
			set_shooting_mode(SHOOTMODE_MOVIE);
		}
		else
		{
			set_shooting_mode(previous_photo_mode);
		}
		return 0;
	}
	
	// enable LiveV stuff in Play mode
	if (event->type == 0 && PLAY_MODE)
	{
		if (event->param == BGMT_UNLOCK)
			livev_playback_toggle();
		else
			livev_playback_reset();
	}

	return 1;
}

static void gui_main_task_60d()
{
	struct event * event = NULL;
	int index = 0;
	void* funcs[GMT_NFUNCS];
	memcpy(funcs, GMT_FUNCTABLE, 4*GMT_NFUNCS);
	gui_init_end();
	while(1)
	{
		msg_queue_receive(gui_main_struct.msg_queue_60d, &event, 0);
		gui_main_struct.counter_60d--;
		if (event == NULL) continue;
		index = event->type;
		if ((index >= GMT_NFUNCS) || (index < 0))
			continue;
		
		if (!magic_is_off())
			if (handle_buttons(event) == 0) 
				continue;
		
		void(*f)(struct event *) = funcs[index];
		f(event);
	}
} 

// 5D2 has a different version for gui_main_task

TASK_OVERRIDE( gui_main_task, gui_main_task_60d );
