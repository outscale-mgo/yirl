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

#include <stdlib.h>
#include <glib.h>
#include <string.h>
#include "entity.h"
#include "debug.h"
#include "tests.h"

static const char *testList[256];
static int testListIdx;
static int list;

#define TEST_TRY_ADD(name, func, only) do	{	\
    if (list)						\
      testList[testListIdx++] = name;			\
    else if (!only || !strcmp(only, name))		\
      g_test_add_func(name, func);			\
  } while (0)

int main(int argc, char **argv)
{
  int only_wid = 0;
  int no_wid = 0;
  char *only = NULL;
  GOptionContext *ctx;
  const GOptionEntry entries[5] = {
    {"no-widgets", 'w', 0,  G_OPTION_ARG_NONE,
     &no_wid, "don't test gui", NULL},
    {"only-widgets", 'W', 0,  G_OPTION_ARG_NONE,
     &only_wid, "test only gui", NULL},
    {"list", 'l', 0, G_OPTION_ARG_NONE, &list,
     "list all the tests and quit", NULL},
    {"just", 'j', 0,  G_OPTION_ARG_STRING, &only,
     "jut do the given test", NULL},
    {NULL, 0, 0, 0, NULL, NULL, NULL}};
  GError *error = NULL;

  ctx = g_option_context_new(NULL);
  g_option_context_set_help_enabled(ctx, 1);
  g_option_context_add_main_entries(ctx, entries, NULL);
  if (!g_option_context_parse(ctx, &argc, &argv, &error)) {
    DPRINT_ERR("option parsing failed: %s\n", error->message);
    g_option_context_free(ctx);
    return 1;
  }
  g_option_context_free(ctx);

  // TODO: add [MY-OPTIONS] -- [TEST-OPTIONS] like syntaxe
  g_test_init(&argc, &argv, NULL);

  yuiDebugInit();

  if (only_wid)
    goto tests_widgets;

  TEST_TRY_ADD("/utils/block-array/lifecycle", testBlockArray, only);

  TEST_TRY_ADD("/entity/lifecycle/simple", testLifecycleSimple, only);
  TEST_TRY_ADD("/entity/lifecycle/flow", testLifecycleFlow, only);
  TEST_TRY_ADD("/entity/lifecycle/complex", testLifecycleComplex, only);
  TEST_TRY_ADD("/entity/lifecycle/akward", testLifecycleAkwarde, only);
  TEST_TRY_ADD("/entity/lifecycle/again", testLifeDeathRebirdAndAgain, only);

  TEST_TRY_ADD("/entity/strings", stringsTests, only);

  TEST_TRY_ADD("/entity/copy", testCopy, only);

  TEST_TRY_ADD("/entity/setunset/simple", testSetSimple, only);
  TEST_TRY_ADD("/entity/setunset/complex", testSetComplex, only);
  TEST_TRY_ADD("/entity/setunset/generic", testSetGeneric, only);

  TEST_TRY_ADD("/script/ybytecode/add", ysciptAdd, only);
  TEST_TRY_ADD("/script/ybytecode/loop", yscriptLoop, only);
  TEST_TRY_ADD("/script/ybytecode/script", ybytecodeScript, only);
  TEST_TRY_ADD("/script/ybytecode/add-function", ybytecodeAddFunction, only);
  TEST_TRY_ADD("/script/ybytecode/loop-function",
  	       ybytecodeLoopCallFunction, only);

  TEST_TRY_ADD("/script/tcc/lifecycle", testTccScritLifecycle, only);
  TEST_TRY_ADD("/script/tcc/add-define", testTccAddDefine, only);
  TEST_TRY_ADD("/script/tcc/macros", testTccTestsMacros, only);

  TEST_TRY_ADD("/script/lua/lifecycle", testLuaScritLifecycle, only);
  TEST_TRY_ADD("/script/lua/entity", testLuaScritEntityBind, only);

  TEST_TRY_ADD("/script/scripts/add-function", testScriptAddFunction, only);

  TEST_TRY_ADD("/parser/json/simple-file", testJsonLoadFile, only);
  TEST_TRY_ADD("/parser/json/complex-file", testJsonMultipleObj, only);
  TEST_TRY_ADD("/parser/json/to-file", testJsonToFile, only);
  /* TEST_TRY_ADD("/sound/soundManager/all", testYSoundLib, only); */

  TEST_TRY_ADD("/modules/list", testListMod, only);
  TEST_TRY_ADD("/modules/dialogue", testDialogueMod, only);

 tests_widgets:
  if (no_wid)
    goto run_test;

#ifdef WITH_SDL
  TEST_TRY_ADD("/widget/lifecycle/sdl", testSdlLife, only);
  TEST_TRY_ADD("/widget/textScreen/sdl", testYWTextScreenSdl2, only);
  TEST_TRY_ADD("/widget/menu/normal/sdl", testYWMenuSdl2, only);
  TEST_TRY_ADD("/widget/menu/panel/sdl", testPanelMenuSdl2, only);
  TEST_TRY_ADD("/widget/map/resized/sdl", testYWMapSdl2, only);
  TEST_TRY_ADD("/widget/map/big/sdl", testYBigWMapSdl2, only);
  TEST_TRY_ADD("/widget/contener/static/horizontal/sdl",
	       testHorizontalContenerSdl, only);
  TEST_TRY_ADD("/widget/contener/static/vertical/sdl", testVerticalContenerSdl,
	       only);
  TEST_TRY_ADD("/widget/contener/static/stack/sdl", testStackContenerSdl, only);
  TEST_TRY_ADD("/widget/contener/static/mix/sdl", testMixContenerSdl, only);
  TEST_TRY_ADD("/widget/contener/dynamic/stack/sdl",
	       testDynamicStackContenerSdl, only);
  TEST_TRY_ADD("/game/sdl/simple", testYGameSdlLibBasic, only);
#endif
#ifdef WITH_CURSES
  TEST_TRY_ADD("/widget/lifecycle/curses", testCursesLife, only);
  TEST_TRY_ADD("/widget/textScreen/curses", testYWTextScreenCurses, only);
  TEST_TRY_ADD("/widget/menu/curses", testYWMenuCurses, only);
  TEST_TRY_ADD("/widget/map/curses", testYWMapCurses, only);
#ifdef WITH_SDL
  TEST_TRY_ADD("/widget/lifecycle/all", testAllLife, only);
  TEST_TRY_ADD("/widget/textScreen/all", testYWTextScreenAll, only);
  TEST_TRY_ADD("/widget/map/all", testYWMapAll, only);
  TEST_TRY_ADD("/game/all/simple", testYGameAllLibBasic, only);
#endif
#endif
  TEST_TRY_ADD("/game/lifecycle", testYGameLifecycle, only);

 run_test:
  if (list) {
    for (int i = 0; i < testListIdx; ++i) {
      printf("%s\n", testList[i]);
    }
  } else {
    g_test_run();
  }
  yuiDebugExit();
}

#undef TEST_TRY_ADD
