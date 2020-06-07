#ifndef __SMX__H_
#define __SMX__H_
#include<stdio.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <psp2/ctrl.h> 

#ifndef __INCLUDED_PLATFORM_INFO__
#define __INCLUDED_PLATFORM_INFO__
#endif
#define COLLECT_NUM 6
enum { ID_ENTER = 1, ID_GAME, ID_START , ID_CREDITS, ID_GAMEOVER , ID_PAUSED };

extern SDL_Renderer *renderer;
extern SDL_Texture *gfx[16], *hgfx[12], *fhgfx[12], *particles[4], *bg, *collect[8], *lsd, *logo;
extern SDL_Surface *sgfx[16], *shgfx[12], *sfhgfx[12], *sparticles[4], *sbg, *scollect[8], *slsd, *slogo;
extern TTF_Font *font;
extern Mix_Chunk *intro_snd , *collect_snd, *fire_snd, *kill_snd;
extern Mix_Music *game_track;
extern int score, lives;
extern void SDL_ReverseBlt(SDL_Surface *surf, SDL_Rect *rc, SDL_Surface *front_surf, SDL_Rect *rc2, Uint32 transparent);
extern int SDL_Colide(SDL_Rect *rc, SDL_Rect *rc2);
extern void init_game();
extern void game_over();
extern void render_enter_level();
extern void render_credits();
extern void render_pause();
extern char *get_path(const char *p, const char *s);
extern Uint32 intro_wait(Uint32 i, void *);
extern Uint32 check_start_in(Uint32, void *);
extern int cur_scr, active, cur_levels;
extern int custom_level;
extern char custom_lvl[256];


#endif


