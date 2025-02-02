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

#ifndef	_YIRL_CONTENER_H_
#define	_YIRL_CONTENER_H_

#include "yirl/widget.h"

typedef enum {
  CNT_VERTICAL,
  CNT_HORIZONTAL,
  CNT_STACK,
  CNT_NONE
} CntType;

typedef enum {
  Y_CNT_GOTO_CURRENT = 0,
  Y_CNT_UNDER_MOUSE = 1,
} YCntForwardingStyle;

typedef struct {
  YWidgetState sate;
  YCntForwardingStyle fwStyle;
  /* Default type is vertical */
  int type;
} YContainerState;

int ywContainerInit(void);
int ywContainerEnd(void);

/**
 * @return 1 if cnt, or a sub idget of cnt contain widget
 */
_Bool ywCntInTree(Entity *cnt, Entity *widget);

/**
 * Create a new widget from @wid and push it
 * @retun the id of the new layer or -1
 */
int ywPushNewWidget(Entity *container, Entity *wid, int dec_ref);

int ywContainerUpdate(Entity *container, Entity *widEnt);

Entity *ywContainerGetWidgetAt(Entity *container, int posX, int posY);

Entity *ywCntGetEntry(Entity *container, int idx);

int ywRemoveEntryByEntity(Entity *container, Entity *target);

int ywReplaceEntry(Entity *container, int idx, Entity *entry);

static inline int
ywReplaceEntryByEntity(Entity *container, Entity *target, Entity *entry)
{
	return ywReplaceEntry(container,
		yeArrayIdx(yeGet(container, "entries"), target),
		entry);
}

void ywCntPopLastEntry(Entity *container);

Entity *ywCntGetLastEntry(Entity *container);

#define ywCntType(opac) (((YContainerState *)opac)->type)

Entity *ywCntWidgetFather(Entity *wid);

#define ywCntMother ywCntWidgetFather
#define ywCntParent ywCntWidgetFather

/**
 * @brief create widget from child that are just entity
 * it is posible to add child to a container at runtime
 * a way to do so is simply to push a new entity in a container "entries"
 * so sometime a container have child that are not yet created widget
 * so this function is usefull to create all childs widgets
 */
void ywCntConstructChilds(Entity *ent);

#endif
