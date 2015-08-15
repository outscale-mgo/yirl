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

#ifndef WIDGET_CALLBACK_H
#define WIDGET_CALLBACK_H

#include "widget.h"

typedef enum {
  YCALLBACK_NATIVE = 0,
  YCALLBACK_NONE = 1
} YCallbackType;

typedef struct {
  int callbackIdx;
  char *name;
} YSignal;

typedef struct {
  char *name;
  /* Can be a entity script to call, or a native func from a widget */
  int type;
} YCallback;

typedef struct {
  YCallback base;
  int (*callack)(YWidgetState *wid, YEvent *eve, Entity *arg);
} YNativeCallback;

int ywidAddSignal(YWidgetState *wid, const char *name);
void ywidFinishSignal(YWidgetState *wid);

int ywidBind(YWidgetState *wid, const char *signal, const char *callback);

int ywinAddCallback(YWidgetState *wid, YCallback *callback);

YCallback *ywinCreateNativeCallback(const char *name,
				    int (*callack)(YWidgetState *wid,
						   YEvent *eve, Entity *arg));

void ywidDdestroyCallback(YWidgetState *wid, int idx);

void ywidFinishCallbacks(YWidgetState *wid);

YCallback * ywinGetCallbackByIdx(YWidgetState *wid, int idx);
YCallback *ywinGetCallbackByStr(YWidgetState *wid, const char *str);

int ywidCallSignal(YWidgetState *wid, YEvent *eve, Entity *arg, unsigned idx);
int ywidCallCallback(YWidgetState *wid, YEvent *eve, Entity *arg, unsigned idx);

#endif
