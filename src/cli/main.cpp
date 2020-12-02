/* main.c - this file is part of DeSmuME
 *
 * Copyright (C) 2006-2015 DeSmuME Team
 * Copyright (C) 2007 Pascal Giard (evilynux)
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <stdlib.h>
#include <string.h>

#ifndef VERSION
#define VERSION "Unknown version"
#endif

#ifndef CLI_UI
#define CLI_UI
#endif

#include "../NDSSystem.h"
#include "../driver.h"
#include "../GPU.h"
#include "../SPU.h"
#include "../sndsdl.h"
#include "../ctrlssdl.h"
#include "../render3D.h"
#include "../rasterize.h"
#include "../saves.h"
#include "../slot2.h"
#include "../utils/xstring.h"

volatile bool execute = false;

float nds_screen_size_ratio = 1.0f;
u16 keypad = 0;

//#define DISPLAY_FPS

#ifndef SDL_TRIPLEBUF
#define SDL_TRIPLEBUF SDL_DOUBLEBUF
#endif

#ifdef DISPLAY_FPS
#define NUM_FRAMES_TO_TIME 60
#endif

#define FPS_LIMITER_FPS 60

static SDL_Surface * surface, *rl_sf;

/* Flags to pass to SDL_SetVideoMode */
static int sdl_videoFlags;

SoundInterface_struct *SNDCoreList[] = {
  &SNDSDL,
  NULL
};

GPU3DInterface *core3DList[] = {
&gpu3DNull,
&gpu3DRasterize,
NULL
};

const char * save_type_names[] = {
    "Autodetect",
    "EEPROM 4kbit",
    "EEPROM 64kbit",
    "EEPROM 512kbit",
    "FRAM 256kbit",
    "FLASH 2mbit",
    "FLASH 4mbit",
    NULL
};


/* Our keyboard config is different because of the directional keys */
/* Please note that m is used for fake microphone */
/* Default Keyboard configuration */

const u16 cli_kb_cfg[NB_KEYS] =
  { 
    SDLK_LCTRL,         // A
    SDLK_LALT,         // B
    SDLK_ESCAPE,    // select
    SDLK_RETURN,    // start
    SDLK_RIGHT,     // Right
    SDLK_LEFT,      	// Left
    SDLK_UP,        	// Up
    SDLK_DOWN,     		// Down
    SDLK_BACKSPACE,  	// R
    SDLK_TAB,         	// L
    SDLK_LSHIFT,         // X
    SDLK_SPACE,         // Y
    SDLK_p,         // DEBUG
    SDLK_o,         // BOOST
    SDLK_z, // Lid
  };

uint32_t frameskip = 3;
uint32_t sdl_quit = 0;

/* Preferably this should draw directly to the surface.
 * I need to test whenever this will draw properly in BGR mode.
 * TODO - Gameblabla
 * */
static void Draw( void)
{
	SDL_BlitSurface(rl_sf, 0, surface, 0);
	SDL_Flip(surface);
	return;
}

static void desmume_cycle()
{
    SDL_Event event;

    /* Look for queued events and update keypad status */
    /* IMPORTANT: Reenable joystick events iif needed. */
    if(SDL_JoystickEventState(SDL_QUERY) == SDL_IGNORE)
      SDL_JoystickEventState(SDL_ENABLE);

    /* There's an event waiting to be processed? */
    while ( !sdl_quit &&
        (SDL_PollEvent(&event) ))
      {
        process_ctrls_event( event, &keypad);
    }

    /* Update mouse position and click */
    if(mouse.down) NDS_setTouchPos(mouse.x, mouse.y);
    if(mouse.click)
      { 
        NDS_releaseTouch();
        mouse.click = FALSE;
      }

    update_keypad(keypad);     /* Update keypad */
    NDS_exec<false>();
    SPU_Emulate_user();
}


int main(int argc, char ** argv) {
	int error;
	int now;

#ifdef DISPLAY_FPS
	u32 fps_timing = 0;
	u32 fps_frame_counter = 0;
	u32 fps_previous_time = 0;
	int limiter_frame_counter = 0;
	int limiter_tick0 = 0;
#endif

	/* the firmware settings */
	struct NDS_fw_config_data fw_config;

	NDS_Init();

	/* default the firmware settings, they may get changed later */
	NDS_FillDefaultFirmwareConfigData( &fw_config);

	/* use any language set on the command line */
	fw_config.language = 1;

    int slot2_device_type = NDS_SLOT2_AUTO;

    /*if (my_config.is_cflash_configured)
        slot2_device_type = NDS_SLOT2_CFLASH;*/

    /*if(my_config.gbaslot_rom != "") {

        //set the GBA rom and sav paths
        GBACartridge_RomPath = my_config.gbaslot_rom.c_str();
        if(toupper(strright(GBACartridge_RomPath,4)) == ".GBA")
          GBACartridge_SRAMPath = strright(GBACartridge_RomPath,4) + ".sav";
        else
          //what to do? lets just do the same thing for now
          GBACartridge_SRAMPath = strright(GBACartridge_RomPath,4) + ".sav";

        // Check if the file exists and can be opened
        FILE * test = fopen(GBACartridge_RomPath.c_str(), "rb");
        if (test) {
            slot2_device_type = NDS_SLOT2_GBACART;
            fclose(test);
        }
    }*/

	switch (slot2_device_type)
	{
		case NDS_SLOT2_NONE:
			break;
		case NDS_SLOT2_AUTO:
			break;
		case NDS_SLOT2_CFLASH:
			break;
		case NDS_SLOT2_RUMBLEPAK:
			break;
		case NDS_SLOT2_GBACART:
			break;
		case NDS_SLOT2_GUITARGRIP:
			break;
		case NDS_SLOT2_EXPMEMORY:
			break;
		case NDS_SLOT2_EASYPIANO:
			break;
		case NDS_SLOT2_PADDLE:
			break;
		case NDS_SLOT2_PASSME:
			break;
		default:
			slot2_device_type = NDS_SLOT2_NONE;
			break;
	}
    
    slot2_Init();
    slot2_Change((NDS_SLOT2_TYPE)slot2_device_type);

	driver = new BaseDriver();

	/* Create the dummy firmware */
	NDS_CreateDummyFirmware( &fw_config);

	//if ( !my_config.disable_sound) {
	SPU_ChangeSoundCore(SNDCORE_SDL, 735 * 4);
	//}

	NDS_3D_ChangeCore(1);

	backup_setManualBackupType(0);

	error = NDS_LoadROM( argv[1] );
	if (error < 0) {
		fprintf(stderr, "error while loading %s\n", argv[1]);
		exit(-1);
	}

	execute = true;

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1)
    {
      fprintf(stderr, "Error trying to initialize SDL: %s\n",
              SDL_GetError());
      return 1;
	}
	SDL_WM_SetCaption("Desmume SDL", NULL);

	surface = SDL_SetVideoMode(256, 384, 16, SDL_HWSURFACE | SDL_TRIPLEBUF);
    rl_sf = SDL_CreateRGBSurfaceFrom((void*)&GPU_screen, 256, 384, 16, 512, 0x001F, 0x03E0, 0x7C00, 0);

    if ( !surface ) {
      fprintf( stderr, "Video mode set failed: %s\n", SDL_GetError( ) );
      exit( -1);
    }

	/* Initialize joysticks */
	if(!init_joy()) return 1;

	load_default_config(cli_kb_cfg);
	
	loadstate_slot(0);

	/*if(my_config.load_slot != -1){
		loadstate_slot(my_config.load_slot);
	}*/

	while(!sdl_quit)
	{
		desmume_cycle();
		//osd->update();
		//DrawHUD();
		Draw();
		//osd->clear();

		for ( uint_fast8_t i = 0; i < frameskip; i++ ) {
			NDS_SkipNextFrame();
			desmume_cycle();
		}

#ifdef LIMITER
#ifdef DISPLAY_FPS
		now = SDL_GetTicks();
#endif
		//if ( !my_config.disable_limiter && !ctrls_cfg.boost)
		{
#ifndef DISPLAY_FPS
			now = SDL_GetTicks();
#endif
			int delay =  (limiter_tick0 + limiter_frame_counter*1000/FPS_LIMITER_FPS) - now;
			if (delay < -500 || delay > 100) { // reset if we fall too far behind don't want to run super fast until we catch up
				limiter_tick0 = now;
				limiter_frame_counter = 0;
			} else if (delay > 0) {
			SDL_Delay(delay);
			}
		}
		// always count frames, we'll mess up if the limiter gets turned on later otherwise
		limiter_frame_counter += 1 + frameskip;

#ifdef DISPLAY_FPS
		fps_frame_counter += 1;
		fps_timing += now - fps_previous_time;
		fps_previous_time = now;

		if ( fps_frame_counter == NUM_FRAMES_TO_TIME)
		{
			char win_title[20];
			float fps = NUM_FRAMES_TO_TIME * 1000.f / fps_timing;

			fps_frame_counter = 0;
			fps_timing = 0;

			snprintf( win_title, sizeof(win_title), "Desmume %f", fps);

			SDL_WM_SetCaption( win_title, NULL);
		}
#endif
#endif

	}

	/* Unload joystick */
	uninit_joy();
  
	if (surface) SDL_FreeSurface(surface);
	if (rl_sf) SDL_FreeSurface(rl_sf);
	SDL_Quit();
	NDS_DeInit();
	return 0;
}
