const int WINDOW_WIDTH = 960;
const int WINDOW_HEIGHT = 544;

#include "level.h"
#include "smx.h"
#define ROUND(x) x - (int) x >= 0.5 ? ((int) x) + 1 : (int) x;

Mix_Chunk *intro_snd = 0, *collect_snd = 0, *fire_snd = 0, *kill_snd = 0;


extern void render_start();


int cur_scr = ID_ENTER;
TTF_Font *font = 0;
SDL_Surface *sgfx[16], *shgfx[12], *sfhgfx[12], *sparticles[4], *sbg, *scollect[8], *slsd, *slogo;
SDL_Texture *gfx[16], *hgfx[12], *fhgfx[12], *particles[4], *bg, *collect[8], *lsd, *logo, *letters[90];
Text *texts[20];
static const char *img_str[] = { "black.bmp", "grass.bmp", "bluebrick.bmp", "bluesky.bmp", "brick.bmp",
"eblock.bmp", "red_brick.bmp", "sand1.bmp", "sand2.bmp", "snow.bmp",
"stone.bmp", "stone2.bmp", "stone3.bmp", "stone4.bmp", "black.bmp",
0 };
static const char *hstr[] = { "hero1.bmp", "hero2.bmp", "hero3.bmp", "hero4.bmp",
"hero_jump1.bmp", "hero_shot1.bmp", "hero_shot2.bmp", "hero_shot3.bmp", "hero_shot4.bmp", 0 };
static const char *ev[] = { "app0:assets/img/grandma/", 0 };
static const char *fev[] = { "app0:assets/img/grandma_flip/", 0 };
Emiter emiter;
int custom_level = 0;
char custom_lvl[256];
int cur_level = 0;
int score = 0, lives = 0;
int active = 1;
int cur_levels = 0;
int menu_level = 0;

Uint32 startTime = 0;
Uint32 endTime = 0;
Uint32 delta = 0;
short fps = 30;
short timePerFrame = 66; // miliseconds

SDL_Window *window;
SDL_Renderer *renderer;

static const char *level_str[] = { "app0:assets/", "app0:assets/SuperMaster2/", 0 };

void reload_level() 
{
	char sbuf[256];
	if(cur_level >= 8)
		cur_level = 0;

	if(custom_level == 0)
	sprintf(sbuf,"%slevel/level%d.sml", level_str[cur_levels], ++cur_level);
	else
	strcpy(sbuf, custom_lvl);
	fprintf(stdout, "Loading next level");
	fflush(stdout);
	if(level != 0) release_level(level);
	level = load_level(sbuf);
	fprintf(stdout, "Level loaded");
	fflush(stdout);

	hero.hpos = level->start_pos;
	srand((unsigned int) SDL_GetTicks() );

	for(int i = 0; i < 50; i++) 
	{
		if(level->items[i].type != 0) do { level->items[i].type = rand()%COLLECT_NUM; } while( level->items[i].type == 0 );
	}
	offset = 0;
	init_particles(&emiter);
	cur_scr = ID_ENTER;
	
}

void init_game() 
{
	score = 0, lives = 10;
	cur_level = 0;
	menu_level = 0;
	cur_scr = ID_START;
}
		
static void init() 
{
	sceCtrlSetSamplingMode(SCE_CTRL_MODE_DIGITAL);
	CreateTexts();
	SDL_SetRenderDrawColor(renderer,0,0,0, 255);
	Uint8 i = 0;
	SDL_RenderCopy(renderer, texts[0]->texture, 0, &texts[0]->rect);
	init_game();
    intro_wait(0,0);
	SDL_RenderCopy(renderer, texts[1]->texture, 0, &texts[1]->rect);

	sparticles[0] = SDL_LoadBMP("app0:assets/img/shot.bmp");
	slsd = SDL_LoadBMP("app0:assets/img/lsd.bmp");
	slogo = SDL_LoadBMP("app0:assets/img/logo.bmp");
	SDL_SetColorKey(sparticles[0] , SDL_TRUE, SDL_MapRGB(sparticles[0]->format, 255, 255, 255));
	lsd = SDL_CreateTextureFromSurface(renderer, slsd);
	logo = SDL_CreateTextureFromSurface(renderer, slogo);
	particles[0] = SDL_CreateTextureFromSurface(renderer, sparticles[0]);
	SDL_FreeSurface(slsd);
	SDL_FreeSurface(slogo);
	SDL_FreeSurface(sparticles[0]);
	for(i = 0; img_str[i] != 0; i++) 
	{
		static char sbuf[256];
		sprintf(sbuf, "app0:assets/img/%s", img_str[i]);
		sgfx[i] = SDL_LoadBMP(sbuf);
		if(!sgfx[i])
			fprintf(stderr, "Error couldnt load graphic %s\n", sbuf);
		else
		{
			SDL_SetColorKey(sgfx[i] , SDL_TRUE, SDL_MapRGB(sgfx[i]->format, 255, 255, 255));
			gfx[i] = SDL_CreateTextureFromSurface(renderer,sgfx[i]);
			SDL_FreeSurface(sgfx[i]);
		}
	}
	for(i = 0; hstr[i] != 0; i++) 
	{
		static char sbuf[256];
		sprintf(sbuf, "app0:assets/img/hero/%s", hstr[i]);
		shgfx[i] = SDL_LoadBMP(sbuf);
		if(!shgfx[i])
			fprintf(stderr, "Error couldnt load graphic %s\n", sbuf);
		else
		{
			SDL_SetColorKey(shgfx[i] , SDL_TRUE, SDL_MapRGB(shgfx[i]->format, 255, 255, 255));
			hgfx[i] = SDL_CreateTextureFromSurface(renderer,shgfx[i]);
			SDL_FreeSurface(shgfx[i]);
		}
	}

	for(i = 0; hstr[i] != 0; i++) 
	{
		static char sbuf[256];
		sprintf(sbuf, "app0:assets/img/hero_flip/%s", hstr[i]);
		sfhgfx[i] = SDL_LoadBMP(sbuf);
		if(!sfhgfx[i])
			fprintf(stderr, "Error couldnt load graphic %s\n", sbuf);
		else
		{
			SDL_SetColorKey(sfhgfx[i] , SDL_TRUE, SDL_MapRGB(sfhgfx[i]->format, 255, 255, 255));
			fhgfx[i] = SDL_CreateTextureFromSurface(renderer,sfhgfx[i]);
			SDL_FreeSurface(sfhgfx[i]);
		}

	}

	for(i = 0; i < COLLECT_NUM; i++) 
	{
		static char sbuf[256];
		sprintf(sbuf, "app0:assets/img/col%d.bmp", i+1);
		scollect[i] = SDL_LoadBMP(sbuf);
		if(!scollect[i])
			fprintf(stderr, "Error couldnt load graphic %s\n", sbuf);
		else
		{
			SDL_SetColorKey(scollect[i] , SDL_TRUE, SDL_MapRGB(scollect[i]->format, 255, 255, 255));
			collect[i] = SDL_CreateTextureFromSurface(renderer,scollect[i]);
			SDL_FreeSurface(scollect[i]);
		}
	}

	for( i = 0; ev[i] != 0; i++ ) 
	{
		for( int z = 0; z < 10; z++ ) 
		{
			static char sbuf[256];
			memset(sbuf, 0, sizeof(sbuf));
			sprintf(sbuf,"%sevil%d.bmp", ev[i], z+1);
			evil_gfx[i].sgfx[z] = SDL_LoadBMP(sbuf);
			if(!evil_gfx[i].sgfx[z])
				fprintf(stdout, "Couldnt load %s : %s\n", sbuf,SDL_GetError());
			else
			{
				SDL_SetColorKey(evil_gfx[i].sgfx[z] , SDL_TRUE, SDL_MapRGB(evil_gfx[i].sgfx[z]->format, 255, 255, 255));
				evil_gfx[i].gfx[z] = SDL_CreateTextureFromSurface(renderer, evil_gfx[i].sgfx[z]);
				SDL_FreeSurface(evil_gfx[i].sgfx[z]);
			}
		}
		evil_gfx[i].type = i;
	}

	for( i = 0; fev[i] != 0; i++ ) 
	{

		for( int z = 0; z < 10; z++ ) 
		{
			static char sbuf[256];
			memset(sbuf, 0, sizeof(sbuf));
			sprintf(sbuf,"%sevil%d.bmp", fev[i], z+1);
			evil_gfx[i].sfgfx[z] = SDL_LoadBMP(sbuf);
			if(!evil_gfx[i].sfgfx[z])
				fprintf(stdout, "Couldnt load %s : %s\n", sbuf,SDL_GetError());
			else
			{
				SDL_SetColorKey(evil_gfx[i].sfgfx[z] , SDL_TRUE, SDL_MapRGB(evil_gfx[i].sfgfx[z]->format, 255, 255, 255));
				evil_gfx[i].fgfx[z] = SDL_CreateTextureFromSurface(renderer, evil_gfx[i].sfgfx[z]);
				SDL_FreeSurface(evil_gfx[i].sfgfx[z]);
			}
		}
	}

	SDL_RenderCopy(renderer, texts[2]->texture, 0, &texts[2]->rect);
	sbg = SDL_LoadBMP("app0:assets/img/bg.bmp");
	bg = SDL_CreateTextureFromSurface(renderer, sbg);
	SDL_FreeSurface(sbg);
	
	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096);
	intro_snd = Mix_LoadWAV("app0:assets/snd/open.wav");
	collect_snd = Mix_LoadWAV("app0:assets/snd/line.wav");
	fire_snd = Mix_LoadWAV("app0:assets/snd/fire.wav");
	kill_snd = Mix_LoadWAV("app0:assets/snd/scream.wav");

	SDL_RenderCopy(renderer, texts[3]->texture, 0, &texts[3]->rect);

	if(intro_snd != 0)
	Mix_PlayChannel( -1, intro_snd, 0);
}


static void render() 
{
	switch(cur_scr) 
	{
		case ID_GAME:
		render_map(level);
		break;
		case ID_ENTER:
		render_enter_level();
		break;
		case ID_START:
		render_start();
		break;
		case ID_CREDITS:
		render_credits();
		break;
		case ID_PAUSED:
		render_pause();
		break;
	}
}

int main(int argc, char **argv) 
{
	Uint32 mode = 0;
	SDL_Surface *ico = 0;


	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0)
		return -1;

	if(TTF_Init()==-1)
	{
		printf("TTF_Init: %s\n", TTF_GetError());
		exit(-2);
	}

    SDL_DisplayMode current;
    
    if(SDL_GetCurrentDisplayMode(0, &current) != 0) 
	{
        fprintf(stderr, "Error could not get display mode: %s", SDL_GetError());
        SDL_Quit();
        exit(-1);
    }

	current.w = WINDOW_WIDTH;
	current.h = WINDOW_HEIGHT;

	SDL_ShowCursor(SDL_FALSE);
	ico = SDL_LoadBMP("app0:assets/img/col1.bmp");
    window = SDL_CreateWindow("No Rick rolls this time :(", 0, 0, current.w, current.h, SDL_WINDOW_SHOWN);
    if(!window) 
	{
        fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        exit(-1);
    }
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if(!renderer) 
	{
        fprintf(stderr, "Error creating Renderer: %s\n", SDL_GetError());
        SDL_Quit();
        exit(-1);
    }

	if(argc == 3 && strcmp(argv[1], "--run") == 0)
	{
		//custom_level = 1;
		memset(custom_lvl, 0, sizeof(custom_lvl));
		strcpy(custom_lvl, argv[2]);
		FILE *fptr = fopen(custom_lvl, "r");
		if(!fptr)
		{
			custom_level = 0;
			fprintf(stderr, "Error level map %s not found!", custom_lvl);
		}
		else 
		{
			custom_level = 1;
			fclose(fptr);
		}
	}
	init();
	
	static SDL_Event e;
	int cx = 1440, cy = 1080;
	
	switch(current.h) 
	{
		case 480:
			cx = 640;
			cy = 480;
			break;
		case 544:
			cx = 960;
			cy = 544;
			break;
		case 720:
			cx = 960;
			cy = 720;
			break;
		case 1080:
			cx = 1440;
			cy = 1080;
			break;
	}

	while(active == 1)
	{
		render();

		if (!startTime) 
			startTime = SDL_GetTicks(); 
		else 
			delta = endTime - startTime; 
		
		if (delta < timePerFrame)
			SDL_Delay(timePerFrame - delta);
		
		if (delta > timePerFrame)
			fps = ROUND(1000 / delta);
		
		startTime = endTime;
		endTime = SDL_GetTicks();

		SDL_RenderPresent(renderer);
		SDL_RenderClear(renderer);
	}
	SDL_Quit();
	return 0;
}

int SDL_Colide(SDL_Rect *rc, SDL_Rect *rc2) 
{
	int i,z;

	if(!(rc->x > 0 && rc->x+rc->w < WINDOW_WIDTH && rc->y > 0 && rc->y+rc->h < WINDOW_HEIGHT))
		return 0;

	for( i = rc->x; i < rc->x+rc->w; i++) 
		{
		for(z = rc->y; z < rc->y+rc->h; z++) 
		{
			if(i >= rc2->x && i <= rc2->x+rc2->w && z >= rc2->y && z <= rc2->y+rc2->h)
				return 1;
		}

	}
	return 0;
}

void CreateTexts()
{
	font = TTF_OpenFont("app0:assets/font/mplus-1mn-bold.ttf", 16);
	if(!font)
	{
		fprintf(stdout, "TTF Error: %s", TTF_GetError());
		fflush(stdout);
		exit(-1);
	}
	SDL_Color White = {255, 255, 255, 255};

	char* messages[]={"Loading...", "Loading Bitmaps...", "Loading Sound Effects", "Done", "SuperMasterX - LostSideDead", "Single Player", "Credits", "Exit", "=)>",
					 "SuperMasterX Play New Game", "New SuperMasterX Levels", "Old SuperMaster2 Levels", "Press Start to Play Level", "\"Open Source - Open Mind\"",
					 "Paused - Press Circle to continue ", "Press Triangle to Return to Menu ", "Ported by Rufis_ @","CBPS"};
	int positionsX[]={25,25,25,25,75,125,125,125,0,75,125,125,150,170,75,75,0,0};
	int positionsY[]={25,25,25,25,75,175,225,275,0,75,175,225,200,380,75,110,400,401};

	SDL_Surface* surfaceMessage;
	for(int i=0; i < sizeof(messages)/sizeof(char *);i++)
	{
		surfaceMessage = TTF_RenderText_Solid(font, messages[i], White);
		texts[i] = malloc(sizeof(Text));
		texts[i]->texture = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
		texts[i]->rect.x = positionsX[i];
		texts[i]->rect.y = positionsY[i];
		texts[i]->rect.w = surfaceMessage->w;
		texts[i]->rect.h = surfaceMessage->h;
	}


	SDL_SetTextureColorMod(texts[6]->texture , 255, 0, 0);
	SDL_SetTextureColorMod(texts[7]->texture , 255, 255, 0);
	SDL_SetTextureColorMod(texts[11]->texture , 255, 0, 0);

	texts[13]->rect.x = (WINDOW_WIDTH - texts[13]->rect.w) / 2;
	texts[16]->rect.x = (WINDOW_WIDTH - (texts[16]->rect.w + texts[17]->rect.w + 2)) / 2;
	texts[17]->rect.x = texts[16]->rect.x + texts[16]->rect.w + 2;

	SDL_FreeSurface(surfaceMessage);

	for(int i = ' '; i<='z'; i++)
	{
		surfaceMessage = TTF_RenderText_Solid(font, (char*)&i, White);
		letters[i - ' '] = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
		SDL_FreeSurface(surfaceMessage);
	}
}

void renderText(char* text, int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	SDL_Rect letter = {x, y, 0, 0};
	while(*text != '\0')
	{
		if(*text >= ' ' && *text <= 'z')
		{
			SDL_QueryTexture(letters[*text - ' '], 0, 0, &letter.w, &letter.h);
			SDL_SetTextureColorMod(letters[*text - ' '], r, g, b);
			SDL_RenderCopy(renderer, letters[*text - ' '], 0, &letter);
			letter.x += letter.w;
		}
		text++;
	}
}




