/*
**Copyright (C) 2019 Matthias Gatto
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

#ifndef _YIRL_EVENTS_H_
#define _YIRL_EVENTS_H_

#include "yirl/entity.h"
#include "yirl/widget.h"

int yevIsKeyDown(Entity *events, int k);
int yevIsKeyUp(Entity *events, int k);

Entity *yevMousePos(Entity *events);

int yevMouseDown(Entity *events, int *button);

#endif
