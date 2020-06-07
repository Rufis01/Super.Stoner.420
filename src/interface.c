#include "smx.h"
#include "level.h"
#include<math.h>

extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;

SceCtrlData controls;
int shown_logo = 0;
int cl_pos = 0, cl2_pos = 0;
extern int menu_level;


Uint32 intro_wait(Uint32 i, void *v)
{
	shown_logo = 1;
	return 0;
}

unsigned char menu_frame = 0;
unsigned char hat_was_pressed = 0;
unsigned char cross_was_pressed = 0;
Uint32 check_start_in(Uint32 i, void *v)
{
	if(cross_was_pressed)
	{
		sceCtrlReadBufferNegative(0, &controls, 1);
		if(controls.buttons & SCE_CTRL_CROSS)
			cross_was_pressed = 0;
	}
	if(hat_was_pressed)
	{
		sceCtrlReadBufferNegative(0, &controls, 1);
		if(controls.buttons & SCE_CTRL_UP && controls.buttons & SCE_CTRL_DOWN)
			hat_was_pressed = 0;
	}
	sceCtrlReadBufferPositive(0, &controls, 1);
    
	if(menu_frame == 0 || !hat_was_pressed)
	{
		if(controls.buttons & SCE_CTRL_UP) 
		{
			if(menu_level == 0 && cl_pos > 0)
			{
				hat_was_pressed = 1;
				menu_frame = 10;
				cl_pos--;
			}
			if(menu_level == 1 && cl2_pos > 0)
			{
				hat_was_pressed = 1;
				menu_frame = 10;
				cl2_pos--;
			}
		}
		else if(controls.buttons & SCE_CTRL_DOWN)
		{
			if(menu_level == 0 && cl_pos < 2)
			{
				hat_was_pressed = 1;
				menu_frame = 10;
				cl_pos++;
			}
			if(menu_level == 1 && cl2_pos < 1)
			{
				hat_was_pressed = 1;
				menu_frame = 10;
				cl2_pos++;
			}
		}
	}
	else if(menu_frame > 0)
	{
		menu_frame--;
	}

	if(controls.buttons & SCE_CTRL_CROSS && !cross_was_pressed)
	{
		cross_was_pressed = 1;
		switch(menu_level) 
		{
			case 0:
			switch(cl_pos) 
			{
				case 0:
					menu_level = 1;
					cl2_pos = 0;
					break;
				case 1:
					cur_scr = ID_CREDITS;
					break;
				case 2:
					{		
						active = 0;
					}
					break;
			}
			break;
			case 1:
				cur_levels = cl2_pos;
				cur_level = 0;
				reload_level();
				return 0;
				break;
		}
	}
	
	return i;
}

void render_start() 
{
	check_start_in(0,0);

	SDL_RenderCopy(renderer, logo, 0, 0);

	if(shown_logo == 1) 
	{

		SDL_Rect rcf = { 50, 50, WINDOW_WIDTH-100, WINDOW_HEIGHT-100 };
		SDL_RenderFillRect(renderer, &rcf);
		int w, h;
		SDL_QueryTexture(hgfx[0], NULL, NULL, &w, &h);
		if(menu_level == 0)
		{
			texts[4]->rect.x = 75;
			texts[4]->rect.y = 75;
			SDL_SetTextureColorMod(texts[4]->texture , rand()%255, rand()%255, rand()%255);
			SDL_RenderCopy(renderer, texts[4]->texture, 0, &texts[4]->rect);
			SDL_RenderCopy(renderer, texts[5]->texture, 0, &texts[5]->rect);
			SDL_RenderCopy(renderer, texts[6]->texture, 0, &texts[6]->rect);//255,0,0
			SDL_RenderCopy(renderer, texts[7]->texture, 0, &texts[7]->rect);//255,255,0
			
			SDL_Rect rc = { 125, WINDOW_HEIGHT-100-h , w, h };
			SDL_RenderCopy(renderer, hgfx[0], 0, &rc );
			texts[8]->rect.x = 100;
			texts[8]->rect.y = 175;
			texts[8]->rect.y = texts[8]->rect.y+(50*cl_pos);
			SDL_SetTextureColorMod(texts[8]->texture , rand()%255, rand()%255, rand()%255);
			SDL_RenderCopy(renderer, texts[8]->texture, 0, &texts[8]->rect);
		}
		else if(menu_level == 1) 
		{
			SDL_SetTextureColorMod(texts[9]->texture , rand()%255, rand()%255, rand()%255);
			SDL_RenderCopy(renderer, texts[9]->texture, 0, &texts[9]->rect);
			SDL_RenderCopy(renderer, texts[10]->texture, 0, &texts[10]->rect);
			SDL_RenderCopy(renderer, texts[11]->texture, 0, &texts[11]->rect);//255,0,0
			
			SDL_Rect rc = { 125, WINDOW_HEIGHT-100-h , w, h };
			SDL_RenderCopy( renderer, hgfx[0], 0, &rc );
			texts[8]->rect.x = 100;
			texts[8]->rect.y = 175;
			texts[8]->rect.y = texts[8]->rect.y+(50*cl2_pos);
			SDL_SetTextureColorMod(texts[8]->texture , rand()%255, rand()%255, rand()%255);
			SDL_RenderCopy(renderer, texts[8]->texture, 0, &texts[8]->rect);
		}
	}
}
unsigned char start_was_pressed = 0;
void check_enter_in() 
{
	sceCtrlReadBufferPositive(0, &controls, 1);
	if(controls.buttons & SCE_CTRL_START)
	{	
		cur_scr = ID_GAME; 	
		start_was_pressed = 1;
	}
}

char sbuf[256], lifebuf[256];

void render_enter_level() 
{
	int w, h;

	SDL_RenderCopy(renderer, bg, 0, 0);
	SDL_QueryTexture(hgfx[0], NULL, NULL, &w, &h);
	SDL_Rect rc = { 50, 50, WINDOW_WIDTH - 100, WINDOW_HEIGHT - 100 };
	SDL_Rect rc2 = { 100, 100, w, h };
	SDL_RenderFillRect(renderer, &rc);
	SDL_RenderCopy( renderer, hgfx[0] , 0, &rc2 );
	sprintf(sbuf, "Now Entering Level %s", level->level_name);
	sprintf(lifebuf, "Lives: %d", lives);
	fflush(stdout);
	
	renderText(sbuf, 150, 100,255,255,rand()%255);
	renderText(lifebuf, 150, 175, 255,255,255);

	SDL_SetTextureColorMod(texts[12]->texture , rand()%255, rand()%255, rand()%255);
	SDL_RenderCopy(renderer, texts[12]->texture, 0, &texts[12]->rect);
	SDL_QueryTexture(lsd, NULL, NULL, &w, &h);
	SDL_Rect rc4 = { 0, WINDOW_HEIGHT-h, w, h };
	SDL_RenderCopy(renderer, lsd, 0, &rc4 );

	check_enter_in();

}

static void credits_in() 
{
	sceCtrlReadBufferPositive(0, &controls, 1);

	if(controls.buttons & SCE_CTRL_CIRCLE)
		cur_scr = ID_START;
}

void render_credits() 
{
	SDL_RenderCopy(renderer, bg, 0, 0);
	credits_in();
	int w, h;
	SDL_QueryTexture(hgfx[4], NULL, NULL, &w, &h);

	SDL_Rect rc = { 100, 300, WINDOW_WIDTH - 200, 125 };
	SDL_Rect rcX = { 125, 345, w, h };

	SDL_RenderFillRect(renderer, &rc);
	SDL_RenderCopy(renderer, hgfx[4], 0, &rcX);
	
	SDL_QueryTexture(evil_gfx[0].gfx[0], NULL, NULL, &w, &h);
	SDL_Rect rcZ = { WINDOW_WIDTH-120-w, 345, w, h };

	SDL_RenderCopy(renderer, evil_gfx[0].fgfx[0], 0, &rcZ);

	SDL_SetTextureColorMod(texts[4]->texture , 255, 255, 255);
	texts[4]->rect.x = (WINDOW_WIDTH - texts[4]->rect.w) / 2;
	texts[4]->rect.y = 360;

	SDL_RenderCopy(renderer, texts[4]->texture, 0, &texts[4]->rect);
	SDL_SetTextureColorMod(texts[13]->texture , rand()%255, rand()%255, rand()%255);
	SDL_RenderCopy(renderer, texts[13]->texture, 0, &texts[13]->rect);

	SDL_RenderCopy(renderer, texts[16]->texture, 0, &texts[16]->rect);
	SDL_SetTextureColorMod(texts[17]->texture , rand()%255, rand()%255, rand()%255);
	SDL_RenderCopy(renderer, texts[17]->texture, 0, &texts[17]->rect);
}
