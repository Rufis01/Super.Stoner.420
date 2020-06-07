#ifndef LEVEL_H__
#define LEVEL_H__

#include<stdio.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#define MAX_TILE 8000

typedef struct _Level	
{
	char level_name[255];
	int grandma[50]; 
	int start_pos;
	struct Tile 
	{
		int block;
		Uint8 solid;
	} tiles[MAX_TILE];
	struct Item 
	{
		int vpos,type;
	} items[50];
} Level;

typedef struct _Hero 
{
	int hpos;
	int x,y,cur_ani,dir;
} Hero;

typedef struct _EvilGfx 
{
	SDL_Texture *gfx[10];
	SDL_Surface *sgfx[10];
	SDL_Texture *fgfx[10];
	SDL_Surface *sfgfx[10];
	int type;
} EvilGfx;

typedef struct _Evil 
{
	int vpos,x,y,cur_ani,dir,type,die;
	EvilGfx *egfx;
} Evil;

typedef struct _Particle 
{
	int x,y,type;
	int vpos,dir;
} Particle;

#define MAX_PARTICLE 10

typedef struct _Emiter 
{
	Particle p[MAX_PARTICLE];
} Emiter;

typedef struct _Text
{
	SDL_Texture * texture;
	SDL_Rect rect;
} Text;

extern int offset;
extern Text *texts[20];
extern Level *level;
extern Emiter emiter;
extern Hero  hero;
extern Evil evil[50];
extern EvilGfx evil_gfx[4];
extern int cur_level;
extern Level *load_level(const char *src);
extern void release_level(Level *);
extern void render_map(Level *lvl);
extern void scroll_left();
extern void scroll_right();
extern Uint32 proccess_game(Uint32 interval, void *);
extern void reload_level();
extern void init_particles(Emiter *);
extern void proc_particles(Emiter *);
extern void rls_particle(Emiter *, int vpos, int type, int dir);
extern void CreateTexts();
extern void renderText(char* text, int x, int y, unsigned char r, unsigned char g, unsigned char b);
extern Uint32 startTime;
extern Uint32 endTime;
extern Uint32 delta;
extern short fps;
extern short timePerFrame; // miliseconds
#endif



