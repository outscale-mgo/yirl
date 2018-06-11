/*
**Copyright (C) 2015 Matthias Gatto
**
**This program is free software: you can redistribute it and/or modify
**it under the terms of the GNU Lesser General Public License as published by
**the Free Software Foundation, either version 3 of the License, or
**(at your option) any later version.
**
**This program is distributed in the hope that it will be useful,
**but WITHOUT ANY WARRANTY; without even the implied warranty of
**MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**GNU General Public License for more details.
**
**You should have received a copy of the GNU Lesser General Public License
**along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include <glib.h>
#include "sdl-internal.h"
#include "yirl/map.h"
#include "yirl/rect.h"
#include "yirl/entity.h"

#define PRINT_BG_AT(pos_at)						\
  if (likely(ywRectContainPos(cam, pos_at, 0))) {			\
    Entity *pos_at##mc = ywMapGetCase(ent, pos_at);			\
    YE_ARRAY_FOREACH(pos_at##mc, mapElem##pos_at) {			\
      if (yeFind(mv_tbl, findEnt, mapElem##pos_at)) {			\
	continue;							\
      }									\
      if (unlikely(sdlDisplaySprites(state, wid, ywPosX(pos_at) - begX,	\
				     ywPosY(pos_at) - begY,		\
				     mapElem##pos_at, sizeSpriteW,	\
				     sizeSpriteH,			\
				     thresholdX, 0, NULL) < 0)) {	\
	sdlConsumeError();						\
      }									\
    }									\
  }									\

static void sdl2MidRender(YWidgetState *state, SDLWid *wid, Entity *ent,
			  int percent);

static Entity *findEnt(const char *useless, Entity *ent, void *ent2)
{
  (void)useless;
  if (yeGet(ent, 2) == ent2)
    return ent2;
  return NULL;
}

int ywMapIsSmoot(Entity *map);

/* crop the map and print the middle of it */
static void sdl2PartialRender(YWidgetState *state, SDLWid *wid, Entity *entity)
{
  Entity *map = yeGet(entity, "map");
  Entity *cam = yeGet(entity, "cam");
  int wMap = ywMapW(entity);
  int hMap = ywMapH(entity);
  int wCam = ywRectW(cam);
  int hCam = ywRectH(cam);
  unsigned int sizeSpriteW;
  unsigned int sizeSpriteH;
  int32_t begX = ywRectX(cam);
  int32_t begY = ywRectY(cam);
  uint32_t thresholdX;

  ywMapGetSpriteSize(entity, &sizeSpriteW, &sizeSpriteH, &thresholdX);

  if (begX < 0)
    begX = 0;
  else if (begX > (wMap - wCam))
    begX = wMap - wCam;

  if (begY < 0)
    begY = 0;
  else if (begY > hMap - hCam)
    begY = hMap - hCam;

  for(int i = 0, curx = 0, cury = 0; i < wCam * hCam; ++i) {
    Entity *mapCase = yeGet(map, begX + curx + ((cury + begY) * wMap));

    YE_ARRAY_FOREACH(mapCase, mapElem) {
      sdlDisplaySprites(state, wid, curx, cury, mapElem,
			sizeSpriteW, sizeSpriteH, thresholdX, 0, NULL);
    }
    ++curx;
    if (curx >= wCam) {
      curx = 0;
      ++cury;
    }
  }
}

static void sdl2MidRender(YWidgetState *state, SDLWid *wid, Entity *ent,
			  int percent)
{
  Entity *gc = yeCreateArray(NULL, NULL);
  Entity *mv_tbl;
  unsigned int sizeSpriteW;
  unsigned int sizeSpriteH;
  uint32_t thresholdX;
  int wMap = ywMapW(ent);
  int hMap = ywMapH(ent);
  Entity *cam = yeGet(ent, "cam");
  int wCam = ywRectW(cam);
  int hCam = ywRectH(cam);
  int32_t begX = ywRectX(cam);
  int32_t begY = ywRectY(cam);
  int32_t endX = begX + wCam;
  int32_t endY = begY + hCam;
  YBgConf cfg;

  if (ywidBgConfFill(yeGet(ent, "background"), &cfg) >= 0) {
    sdlFillBg(wid, &cfg);
  }

  sdl2PartialRender(state, wid, ent);
  if (!ywMapIsSmoot(ent))
    return;

  if (begX < 0) {
    begX = 0;
    endX = begX + wCam;
  } else if (endX > wMap) {
    begX = wMap - wCam;
    endX = wMap;
  }

  if (begY < 0) {
    begY = 0;
    endY = begY + hCam;
  } else if (endY > hMap) {
    begY = hMap - hCam;
    endY = hMap;
  }

  ywMapGetSpriteSize(ent, &sizeSpriteW, &sizeSpriteH, &thresholdX);
  mv_tbl = yeGet(ent, "$mv_tbl");
  if (yeLen(mv_tbl))
    state->hasChange = 1;
  YE_ARRAY_FOREACH(mv_tbl, tbl) {
    Entity *from = yeGet(tbl, 0);
    Entity *to = yeGet(tbl, 1);
    Entity *seg = ywPosDoPercent(ywSegmentFromPos(from, to,
                                                  gc, NULL),
                                 percent);
    ywPosAdd(from, seg);
    {PRINT_BG_AT(from)};
    ywPosAddXY(from, 0, 1); // 0/1
    {PRINT_BG_AT(from)};
    ywPosAddXY(from, 1, 0); // 1/1
    {PRINT_BG_AT(from)};
    ywPosAddXY(from, -2, 0); // -1/1
    {PRINT_BG_AT(from)};
    ywPosAddXY(from, 0, -1); // -1/0
    {PRINT_BG_AT(from)};
    ywPosAddXY(from, 2, 0); // 1/0
    {PRINT_BG_AT(from)};
    ywPosAddXY(from, 0, -1); // 1/-1
    {PRINT_BG_AT(from)};
    ywPosAddXY(from, -1, 0); // 0/-1
    {PRINT_BG_AT(from)};
    ywPosAddXY(from, -1, 0); // -1/-1
    {PRINT_BG_AT(from)};
    ywPosAddXY(from, 1, 1); // 0/0
    yeClearArray(gc);
  }


  YE_ARRAY_FOREACH(mv_tbl, tbl2) {
    Entity *from = yeGet(tbl2, 0);
    Entity *to = yeGet(tbl2, 1);
    Entity *seg = ywPosDoPercent(ywPosMultXY(ywSegmentFromPos(from, to,
							      gc, NULL),
					     sizeSpriteW, sizeSpriteH),
				 percent);
    Entity *movingElem = yeGet(tbl2, 2);
    Entity *modifier = NULL;

    if (!ywRectContainPos(cam, from, 0)) {
      if (!ywRectContainPos(cam, to, 0))
	continue;
      modifier = yeCreateArray(gc, NULL);
      // type of modifier, 0 because for now is the only modifier
      yeCreateInt(0, modifier, NULL);
      if (begX == ywPosX(to) && ywPosX(to) > ywPosX(from)) {
      	yeCreateInts(modifier, yuiPercentOf(sizeSpriteW, 100 - percent),
      		     yuiPercentOf(sizeSpriteW, 100 - percent), 100 - percent,
		     100 - percent);
      }
      else if (endX == ywPosX(from) && ywPosX(to) < ywPosX(from)) {
      	yeCreateInts(modifier, 0, yuiPercentOf(sizeSpriteW, 100 - percent), 0,
		     100 - percent);
      }
    } else if (!ywRectContainPos(cam, to, 0)) {
      modifier = yeCreateArray(gc, NULL);
      // type of modifier, 0 because for now is the only modifier
      yeCreateInt(0, modifier, NULL);
      if (begX == ywPosX(from) && ywPosX(to) < ywPosX(from)) {
      	yeCreateInts(modifier, yuiPercentOf(sizeSpriteW, percent),
      		     yuiPercentOf(sizeSpriteW, percent), percent,
		     percent);
      } else if (endX == ywPosX(to) && ywPosX(to) > ywPosX(from)) {
      	yeCreateInts(modifier, 0, yuiPercentOf(sizeSpriteW, percent), 0,
		     percent);
      }
    }

    if (unlikely(sdlDisplaySprites(state, wid, ywPosX(from) - begX,
				   ywPosY(from) - begY,
				   movingElem, sizeSpriteW, sizeSpriteH,
				   thresholdX + ywPosX(seg),
				   ywPosY(seg), modifier) < 0)) {
      sdlConsumeError();
    }

    yeClearArray(gc);
  }
  yeDestroy(gc);
}

/* rend all the map, regardeless if the map is bigger than the screen */
static int sdl2FullRender(YWidgetState *state, SDLWid *wid, Entity *entity)
{
  Entity *map = yeGet(entity, "map");
  unsigned int lenMap = ywMapLen(entity);
  unsigned int wMap = ywMapW(entity);
  YBgConf cfg;
  unsigned int hMap = lenMap / wMap;
  unsigned int sizeSpriteW;
  unsigned int sizeSpriteH;
  uint32_t thresholdX;

  if (unlikely(!hMap || !wMap || !yeLen(map))) {
    DPRINT_ERR("can't rend empty map\n");
    return -1;
  }

  if (ywidBgConfFill(yeGet(entity, "background"), &cfg) >= 0) {
    sdlFillBg(wid, &cfg);
  }

  ywMapGetSpriteSize(entity, &sizeSpriteW, &sizeSpriteH, &thresholdX);

  YE_ARRAY_FOREACH_EXT(map, mapCase, it) {
    unsigned int curx = yBlockArrayIteratorIdx(it) % wMap;
    unsigned int cury = yBlockArrayIteratorIdx(it) / wMap;

    YE_ARRAY_FOREACH(mapCase, mapElem) {
      if (unlikely(sdlDisplaySprites(state, wid, curx, cury, mapElem,
				     sizeSpriteW, sizeSpriteH, thresholdX,
				     0, NULL) < 0)) {
	sdlConsumeError();
      }
    }
  }

  sdl2MidRender(state, wid, state->entity, 0);
  return 0;
}

static int sdl2Render(YWidgetState *state, int t)
{
  SDLWid *wid = ywidGetRenderData(state, t);
  Entity *ent = state->entity;

  if (ywMapType(ent) != YMAP_PARTIAL)
    return sdl2FullRender(state, wid, ent);
  sdl2MidRender(state, wid, ent, 0);
  return 0;
}

static int sdl2Init(YWidgetState *wid, int t)
{
  wid->renderStates[t].opac = g_new(SDLWid, 1);
  sdlWidInit(wid, t);
  return 0;
}


static void midRender(YWidgetState *state, int t, int percent)
{
  SDLWid *wid = ywidGetRenderData(state, t);
  Entity *ent = state->entity;

  return sdl2MidRender(state, wid, ent, percent);
}

int ysdl2RegistreMap(void)
{
  int ret = ywidRegistreTypeRender("map", ysdl2Type(),
				   sdl2Render, sdl2Init, sdlWidDestroy);
  ywidRegistreMidRend(midRender, ret, ysdl2Type());
  return ret;
}

#undef PRINT_BG_AT
