#include "level.h"
#include "smx.h"

extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;

const int LEVEL_VERTICAL_TILES = 24;
const int COLUMNS_TO_RENDER = 50;

Level *level;
Hero  hero;
Evil  evil[50];
EvilGfx evil_gfx[4];

int offset = 0;
static int jump = 0, jump_ok = 0;
int hero_ani = 0;
void logic();

static int shoot_ani = 0;

unsigned char fps_update_frame = 0;

static void fill_evil(Evil *e, int vpos, int type) 
{
	e->vpos = vpos;
	e->type = type;
	e->dir = 0;
	e->egfx = &evil_gfx[e->type];
}

Level *load_level(const char *src) 
{
	Level *lvl = 0;
	FILE *fptr = 0;

	fptr = fopen(src, "rb");
	if(!fptr)
		return 0;

	lvl = (Level*) malloc ( sizeof( Level ) );

	memset( lvl, 0, sizeof(Level) );

	fread( (char*)lvl, 1, sizeof(Level), fptr);

	fclose(fptr);

	hero.hpos = lvl->start_pos;
	hero.cur_ani = 0;
	hero.dir = 1;
	
	memset(evil, 0, sizeof(evil));
	for(int i = 0; i < 50; i++)
	{
		if(lvl->grandma[i] != 0) 
		{
			fill_evil(&evil[i], lvl->grandma[i], 0); // fill with different types of level objects
		}
		else
			evil[i].type = -1;
	}

	return lvl;
}


void release_level(Level *lvl) 
{
	free(lvl);
}


void render_pause() 
{
	SceCtrlData controls;
	sceCtrlReadBufferPositive(0, &controls, 1);

	SDL_Rect rc = { 50,50,WINDOW_WIDTH-100,WINDOW_HEIGHT-100 };
	if(controls.buttons & SCE_CTRL_CIRCLE) 
	{
		cur_scr = ID_GAME;
	}

	if(controls.buttons & SCE_CTRL_TRIANGLE) 
	{
		game_over();
	}

	SDL_RenderCopy(renderer, bg, 0, 0);
	SDL_RenderFillRect(renderer, &rc);
	SDL_RenderCopy(renderer, texts[14]->texture, 0, &texts[14]->rect);
	SDL_RenderCopy(renderer, texts[15]->texture, 0, &texts[15]->rect);//255,0,0
}

char buf[256];
char fpsbuf[256];
extern unsigned char start_was_pressed;
void render_map(Level *lvl)
{
	proccess_game(0,0);
	SceCtrlData controls;
	if(start_was_pressed)
	{
		sceCtrlReadBufferNegative(0, &controls, 1);
		if(controls.buttons & SCE_CTRL_START)
			start_was_pressed = 0;
	}

	sceCtrlReadBufferPositive(0, &controls, 1);

	static int startby;
	static int bx,by;
	int gcount = 0;
	Uint32 i;
	startby = 75;
	bx = 75; by = startby;

	if(controls.buttons & SCE_CTRL_START && !start_was_pressed)
	{
		cur_scr = ID_PAUSED;
		return;
	}

	SDL_RenderCopy(renderer, bg, 0, 0);

	logic();

	for(i = 0; i < LEVEL_VERTICAL_TILES * COLUMNS_TO_RENDER ; i++)
	{
		SDL_Rect rc = { bx, by, 16, 16 };
		if(offset+i < MAX_TILE)
			SDL_RenderCopy(renderer, gfx[lvl->tiles[offset+i].block], 0, &rc);
		else
			SDL_RenderCopy(renderer, gfx[lvl->tiles[MAX_TILE - 1].block], 0, &rc);
				
		by = by + 16;
		gcount++;
		if(gcount >= LEVEL_VERTICAL_TILES)
		{
			gcount = 0;
			by = startby;
			bx = bx + 16;
		}			
	}

	SDL_Rect rc = { 25, 15, WINDOW_WIDTH-50, 30 };
	SDL_RenderFillRect(renderer, &rc);
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "Score: %d Lives: %d", score, lives);

	TTF_SizeText(font, buf, &rc.w, 0);
	renderText(buf,(WINDOW_WIDTH - rc.w)/2,20,rand()%255,rand()%255,rand()%255);
	renderText(lvl->level_name,35,20,255,255,255);

	if(fps_update_frame++ >= 5)
	{
		sprintf(fpsbuf, "FPS: %hd/15", fps);
		renderText(fpsbuf, (WINDOW_WIDTH - 175), 20, 128 + rand()%127, 0, 0);
		fps_update_frame = 0;
	}
	else
	{
		renderText(fpsbuf, (WINDOW_WIDTH - 175), 20, 128 + rand()%127, 0, 0);
	}

	startby = 75;
	bx = 75; by = startby;
	gcount = 0;
	hero.x = hero.y = 0;

	if(hero_ani == 1) 
	{
		static int w = 0;
		if(w++ >= 3) w = 0;
		hero.cur_ani = w;
	}
	int w, h;
	for( i = 0; i <  LEVEL_VERTICAL_TILES * COLUMNS_TO_RENDER ; i++)//Not necessary(?)
	{
		SDL_QueryTexture(hgfx[hero.cur_ani], 0, 0, &w, &h);
		SDL_Rect rc = { bx, by, w-1, h };
		if(i == hero.hpos) 
		{
			hero.x = bx, hero.y = by;
			if(hero.dir == 1)
				SDL_RenderCopy(renderer,  hgfx[hero.cur_ani], 0, &rc);
			else 
			{
				SDL_RenderCopy(renderer,  fhgfx[hero.cur_ani], 0, &rc);
			}
		}
		
		for(int pos = 0; pos < MAX_PARTICLE; pos++) 
		{
			if(emiter.p[pos].type != 0 && emiter.p[pos].vpos == i+offset) 
			{
				SDL_QueryTexture(particles[0], 0, 0, &w, &h);
				SDL_Rect rcX = { bx, by, w, h };
				SDL_RenderCopy(renderer, particles[0], 0, &rcX );
				emiter.p[pos].x = bx, emiter.p[pos].y = by;
			}
		}

		for(int pos = 0; pos < 50; pos++) 
		{		
			if(evil[pos].type != -1 && evil[pos].vpos == i+offset) 
			{
				SDL_QueryTexture(evil[pos].egfx->gfx[evil[pos].type], 0, 0, &w, &h);
				SDL_Rect erc = { 0,  0, w-1, h };
				SDL_Rect prc = { bx, by, w, h };
				if(evil[pos].dir == 1) 
				{
					SDL_RenderCopy(renderer, evil[pos].egfx->gfx[evil[pos].cur_ani], 0, &prc);
				}
				else
					SDL_RenderCopy(renderer, evil[pos].egfx->fgfx[evil[pos].cur_ani], 0, &prc);

				evil[pos].x = bx, evil[pos].y = by;
			}

			if(level->items[pos].type != 0 && level->items[pos].vpos == i+offset) 
			{
				SDL_QueryTexture(collect[level->items[pos].type], 0, 0, &w, &h);
				SDL_Rect rc = { bx, by, w, h };
				SDL_RenderCopy(renderer, collect[level->items[pos].type], 0, &rc);
			}
		}
		by = by + 16;
		gcount++;
		if(gcount >= LEVEL_VERTICAL_TILES) 
		{
			gcount = 0;
			by = startby;
			bx = bx + 16;
		}
	}

	if(level->tiles[hero.hpos+offset].block == 14)
	{
		reload_level();
	}
	if(lives < 0)
		game_over();
}

void scroll_left() 
{
	if(offset > 0)
		offset -= 24;
}


void scroll_right() 
{
	if(offset < MAX_TILE)
		offset += 24;
}



static void move_left() 
{
	hero.dir = 0;
	if(hero.hpos > 0 && offset == 0) 
	{
			Uint8 check[5];
			check[0] = level->tiles[hero.hpos-24].solid;
			check[1] = level->tiles[hero.hpos+1-24].solid;
			check[2] = level->tiles[hero.hpos+2-24].solid;
			check[3] = level->tiles[hero.hpos+3-24].solid;
			check[4] = level->tiles[hero.hpos-24-24].solid;
			if(!check[0] && !check[1] && !check[2] && !check[3] && !check[4]) hero.hpos -= 24;
			hero_ani = 1;
	}
	else 
	{
			Uint8 check[5];

			check[0] = level->tiles[hero.hpos+offset-24].solid;
			check[1] = level->tiles[hero.hpos+offset+1-24].solid;
			check[2] = level->tiles[hero.hpos+offset+2-24].solid;
			check[3] = level->tiles[hero.hpos+offset+3-24].solid;
			check[4] = level->tiles[hero.hpos+offset-24-24].solid;
			if(!check[0] && !check[1] && !check[2] && !check[3] && !check[4]) scroll_left();
			hero_ani = 1;

	}
}

static void move_right() 
{
	Uint8 check[5];
	hero.dir = 1;
	if(hero.hpos < LEVEL_VERTICAL_TILES*15) 
	{
		check[0] = level->tiles[hero.hpos + 27].solid;
		check[1] = level->tiles[hero.hpos + 27 + 24].solid;
		check[2] = level->tiles[hero.hpos + 27 + 23].solid;
		check[3] = level->tiles[hero.hpos + 27 + 22].solid;
		check[4] = level->tiles[hero.hpos + 27 + 21].solid;

		if(!check[0] && !check[1] && !check[2] && !check[3] && !check[4]) hero.hpos += 24;
		hero_ani = 1;
		if(jump_ok == 0 && jump == 0 && hero_ani == 0 && shoot_ani == 0)
			hero.cur_ani = 0;
		
	}
	else 
	{
		check[0] = level->tiles[hero.hpos + 27+offset].solid;
		check[1] = level->tiles[hero.hpos + 27 + 24+offset].solid;
		check[2] = level->tiles[hero.hpos + 27 + 23+offset].solid;
		check[3] = level->tiles[hero.hpos + 27 + 22+offset].solid;
		check[4] = level->tiles[hero.hpos + 27 + 21+offset].solid;

		if(!check[0] && !check[1] && !check[2] && !check[3] && !check[4]) scroll_right();
		hero_ani = 1;
	}
}

static void rls_bullet() 
{
	if(hero.dir == 1)
		rls_particle(&emiter, offset+hero.hpos+24+24+1, 1, hero.dir);
	else
		rls_particle(&emiter, offset+hero.hpos-24+1, 1, hero.dir);

	if(fire_snd != 0)
	Mix_PlayChannel( -1, fire_snd, 0);
}

static int check_input() 
{
	SceCtrlData controls;
	sceCtrlReadBufferPositive(0, &controls, 1);
	static int w = 0;

	if((controls.buttons & SCE_CTRL_CROSS) && jump_ok == 1 && jump == 0)
		jump = 1, shoot_ani = 0;

	if((controls.buttons & SCE_CTRL_CIRCLE) && jump_ok == 1 && jump == 0)
	{
        if(shoot_ani == 0)
		{
			shoot_ani = 1, hero.cur_ani = 5;
		}
	}
    
	if(controls.buttons & SCE_CTRL_LEFT)
	{
/*		if(shoot_ani == 0) */ move_left();
		return 0;
	}
	
	if(controls.buttons & SCE_CTRL_RIGHT)
	{
/*		if(shoot_ani == 0)  */ move_right();
		return 0;
	}
	return 1;
}

static void collect_item(int type) 
{
	score += 10*type;
	if(collect_snd != 0)
		Mix_PlayChannel( -1, collect_snd, 0);
}

static void proc_collect() 
{
	for(int i = 0; i < 50; i++) 
	{
		if(level->items[i].type != 0) 
		{
			for(int z = 0; z < 4; z++) 
			{
				if((offset+hero.hpos+24+z == level->items[i].vpos) || (offset+hero.hpos-24-z == level->items[i].vpos) || (offset+hero.hpos+z == level->items[i].vpos) || offset+hero.hpos-z == level->items[i].vpos )
				{

					collect_item(level->items[i].type);
					level->items[i].type = 0;
					level->items[i].vpos = 0;
					return;
				}
			}
		}
	}
}

Uint32 proccess_game(Uint32 interval, void *p) 
{
	SceCtrlData controls;
	sceCtrlReadBufferNegative(0, &controls, 1);

	if(level == 0)
		return interval;

	int x = check_input();

	proc_collect();
	proc_particles(&emiter);

	if(jump == 0) 
	{
		if((! level->tiles[hero.hpos+offset+4].solid )&& (!level->tiles[hero.hpos+offset+4+24].solid))
		{
			hero.hpos++;
			hero.cur_ani = 4;
			jump_ok = 0;
			shoot_ani = 0;
		}
		else 
		{
			hero_ani = 1;
			if(shoot_ani == 0) 
			{
				jump_ok = 1; jump = 0; 
				hero.cur_ani = 0;
			}
		}
	}
	else 
	{
		jump++;
		if(hero.hpos > 0 && !level->tiles[hero.hpos+offset-1].solid)
			hero.hpos--;
		else
		{
			/*jump = 0;		//Required to beat 2nd level!!!
			jump_ok = 0;*/
		}
		
		hero.cur_ani = 4;

		if(jump > 12 || controls.buttons & SCE_CTRL_CROSS)
		{
			jump = 0;
			jump_ok = 0;
		}

	}
	
	if(x == 1)
		hero_ani = 0;

	return interval;
}

void logic() 
{
	if(shoot_ani == 0 && hero_ani == 0 && jump_ok == 0 && jump == 0)
		hero.cur_ani = 4;
	else if(shoot_ani == 1 && hero_ani == 0 ) 
	{
		 hero.cur_ani++;
		 if(hero.cur_ani > 8) 
		{
			rls_bullet();
			hero.cur_ani = 0;
			shoot_ani = 0;
			hero_ani = 0;
		}
	}
	else if(hero_ani == 1 && jump_ok == 0)
	{
		hero_ani = 0;
	}
}

void init_particles(Emiter *e) 
{
	memset(e, 0, sizeof(Emiter));
}

void game_over() 
{
	init_game();
}

static void hero_die() 
{
	Level *lvl = level;
	offset = 0;
	hero.hpos = level->start_pos;
	memset(&emiter, 0, sizeof(emiter));
	memset(evil, 0, sizeof(evil));

	for(int i = 0; i < 50; i++)
	{	if(lvl->grandma[i] != 0) 
		{
			fill_evil(&evil[i], lvl->grandma[i], 0); // fill with different types of level objects
		}
		else
			evil[i].type = -1;
	}

	lives--;
	hero_ani = 0;
	shoot_ani = 0;
	jump = 0;
	jump_ok = 1;
	
}

void proc_particles(Emiter *e) 
{
	unsigned int i = 0;

	for( i = 0; i < MAX_PARTICLE; i++) 
	{
		if(e->p[i].type != 0) 
		{
			
			if(e->p[i].vpos >= MAX_TILE-24 || e->p[i].vpos <= 24 ||  level->tiles[e->p[i].vpos].solid)
			{
				e->p[i].type = 0;
				continue;
			}

			if(e->p[i].dir == 0) 
				e->p[i].vpos -= 24;
			else
				e->p[i].vpos += 24;
			
		}
	}


	for( i = 0; i < 50; i++ ) 
	{

		Uint8 check[5];

		if(evil[i].type != -1) 
		{
			if(!level->tiles[evil[i].vpos+4].solid)
				evil[i].vpos++;

		if(evil[i].die == 0) 
		{
			evil[i].cur_ani ++;
			if(evil[i].cur_ani >= 5)
			evil[i].cur_ani = 0;

		}
		else 
		{
			evil[i].cur_ani++;
			if(evil[i].cur_ani > 7) 
			{
				evil[i].type = -1;
				if(kill_snd != 0)
					Mix_PlayChannel(-1, kill_snd, 0);
				break;
			}
		}

		if(evil[i].dir == 0)
		{
			check[0] = level->tiles[evil[i].vpos-24].solid;
			check[1] = level->tiles[evil[i].vpos+1-24].solid;
			check[2] = level->tiles[evil[i].vpos+2-24].solid;
			check[3] = level->tiles[evil[i].vpos+3-24].solid;
			check[4] = level->tiles[evil[i].vpos-24-24].solid;
			if(!check[0] && !check[1] && !check[2] && !check[3] && !check[4]) evil[i].vpos -= 24; else evil[i].dir = 1;

		}
		else if(evil[i].dir == 1)
		{

			check[0] = level->tiles[evil[i].vpos + 27].solid;
			check[1] = level->tiles[evil[i].vpos + 27 + 24].solid;
			check[2] = level->tiles[evil[i].vpos + 27 + 23].solid;
			check[3] = level->tiles[evil[i].vpos + 27 + 22].solid;
			check[4] = level->tiles[evil[i].vpos + 27 + 21].solid;

			if(!check[0] && !check[1] && !check[2] && !check[3] && !check[4]) 
				evil[i].vpos += 24;
			else
				evil[i].dir = 0;
		}

		// check cords agianst hero cords to see if they clip
		if(evil[i].type != -1 && hero.x > 0 && hero.y > 0 && evil[i].x > 0 && evil[i].y > 0)
		{
			int w,h;
			SDL_QueryTexture(evil[i].egfx->gfx[evil[i].cur_ani],0,0,&w,&h);
			SDL_Rect rcY = { evil[i].x, evil[i].y, w, h };
			SDL_QueryTexture(hgfx[hero.cur_ani],0,0,&w,&h);
			SDL_Rect rcX = { hero.x, hero.y, w, h };
			if(SDL_Colide(&rcX, &rcY))
			{
				hero_die();
				return;
			}
		}
			int w,h, pw, ph;
			SDL_QueryTexture(particles[0],0,0,&pw,&ph);
			for(Uint8 p = 0; p < MAX_PARTICLE; p++)
			{
				if(e->p[p].type != 0)				
				{
					if(e->p[p].x > 0 && e->p[p].y > 0 && e->p[p].x < WINDOW_WIDTH && e->p[p].y < WINDOW_HEIGHT && evil[i].x > 0 && evil[i].y > 0 && evil[i].x < WINDOW_WIDTH && evil[i].y < WINDOW_HEIGHT)
					{
						SDL_Rect rcX = { e->p[p].x, e->p[p].y, pw, ph };
						SDL_QueryTexture(evil[i].egfx->gfx[evil[i].cur_ani],0,0,&w,&h);
						SDL_Rect rcY = { evil[i].x, evil[i].y, w, h };
						if(SDL_Colide(&rcX, &rcY))
						{	//TODO: FIX bullet through paper
							//memset(&e->p[p], 0, sizeof(e->p[i]));
							e->p[p].type = 0;
							if(!evil[i].die)
							{
								evil[i].die = 1;
								evil[i].cur_ani = 5;
								score++;
							}
						}
					}
				}
			}
		}
	}
}

static int get_off_particle(Emiter *e) 
{
	unsigned int i = 0;
	for ( i = 0; i < MAX_PARTICLE; i++ ) 
	{
		if(e->p[i].type == 0)
		return (int)i;
	}
	return -1;
}

void rls_particle(Emiter *e, int vpos, int type, int dir) 
{
	int off = get_off_particle(e);
	if(off != -1) 
	{
		e->p[off].type = type;
		e->p[off].vpos = vpos;
		e->p[off].dir = dir;
	}
}
