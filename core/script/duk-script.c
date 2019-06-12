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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <glib.h>

#include <src/duktape.h>
#include <extras/print-alert/duk_print_alert.h>
#include <extras/console/duk_console.h>

#include "game.h"
#include "canvas.h"
#include "widget.h"
#include "pos.h"
#include "entity-script.h"
#include "events.h"

static int t = -1;

#define OPS(s) ((YScriptDuk *)s)->ops
#define CTX(s) ((YScriptDuk *)s)->ctx

#define GET_E(ctx, i)				\
	duk_get_pointer(ctx, i)

#define PUSH_E(c, e)				\
	duk_push_pointer(c, e)


typedef struct {
	YScriptOps ops;
	duk_context *ctx;
} YScriptDuk;

static duk_ret_t dukto_str(duk_context *ctx)
{
	if (duk_is_pointer(ctx, 0)) {
		void *p = duk_get_pointer(ctx, 0);
		duk_push_string(ctx, (char *)p);
	} else {
		duk_push_null(ctx);
	}
	return 1;
}

static duk_ret_t dukyeCreateString(duk_context *ctx)
{
	PUSH_E(ctx, yeCreateString(duk_get_string(ctx, 0),
				   GET_E(ctx, 1),
				   duk_get_string(ctx, 2)));
	return 1;
}

#define BIND_I_E(x, u0, u1)					\
	static duk_ret_t duk##x(duk_context *ctx) {		\
		duk_push_int(ctx, x(GET_E(ctx, 0)));		\
		return 1;					\
	}

#define DUMB_FUNC(x)						\
	static duk_ret_t duk##x(duk_context *ctx) {		\
		printf("BIND not yet implemented %s\n", #x);	\
		return 0;					\
	}

DUMB_FUNC(ywPosCreate);
DUMB_FUNC(yeGet);
DUMB_FUNC(yeGetIntAt);
DUMB_FUNC(yeSetIntAt);
DUMB_FUNC(yevCreateGrp);
DUMB_FUNC(yesCall);
DUMB_FUNC(yeIncrAt);
DUMB_FUNC(yeAddAt);

#define BIND_I_V(a) DUMB_FUNC(a)
#define BIND_S_E(a, b, c) DUMB_FUNC(a)
#define BIND_E_S(a, b, c) DUMB_FUNC(a)
#define BIND_V_E(a, b, c) DUMB_FUNC(a)
#define BIND_V_EI(a, b, c) DUMB_FUNC(a)
#define BIND_V_EE(a, b, c) DUMB_FUNC(a)
#define BIND_V_EII(a, b, c) DUMB_FUNC(a)
#define BIND_E_E(a, b, c) DUMB_FUNC(a)
#define BIND_E_EE(a, b, c) DUMB_FUNC(a)
#define BIND_E_ES(a, b, c) DUMB_FUNC(a)
#define BIND_E_EI(a, b, c) DUMB_FUNC(a)
#define BIND_B_EE(a, b, c) DUMB_FUNC(a)
#define BIND_B_EES(a, b, c) DUMB_FUNC(a)
#define BIND_B_EEE(a, b, c) DUMB_FUNC(a)
#define BIND_E_EIIE(a, b, c) DUMB_FUNC(a)
#define BIND_E_EIIS(a, b, c) DUMB_FUNC(a)
#define BIND_B_EEEE(a, b, c) DUMB_FUNC(a)
#include "binding.c"

static void *call(void *sm, const char *name, va_list ap)
{
	duk_context *ctx = CTX(sm);
	int i = 0;

	duk_get_global_string(ctx, name);
	for (void *tmp = va_arg(ap, void *); tmp != Y_END_VA_LIST;
	     tmp = va_arg(ap, void *)) {
		if (!yeIsPtrAnEntity(tmp))
			PUSH_E(ctx, tmp);
		else if ((intptr_t)tmp < 65536)
			duk_push_int(ctx, (intptr_t)tmp);
		else
			duk_push_pointer(ctx, tmp);
		++i;
	}
	duk_call(ctx, i);
	i = duk_get_top(ctx) - 1;
	if (duk_is_pointer(ctx, i))
		return duk_get_pointer(ctx, i);
	if (duk_is_boolean(ctx, i))
		return (void *)duk_get_boolean(ctx, i);
	return (void *)duk_get_int(ctx, i);
}

static int destroy(void *sm)
{
	duk_destroy_heap(CTX(sm));
	free(sm);
	return 0;
}

static int init(void *sm, void *args)
{
	duk_context *ctx = duk_create_heap_default();

	duk_print_alert_init(ctx, 0);
	duk_console_init(ctx, 0);
	CTX(sm) = ctx;
	duk_push_global_object(ctx);

#define BIND(x, args, oargs)						\
	duk_push_string(ctx, #x);					\
	duk_push_c_function(ctx,					\
			    duk##x, args + oargs ?			\
			    args + oargs : DUK_VARARGS);		\
	duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE |			\
		     DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

#define PUSH_I_GLOBAL(X)
#define IN_CALL 1

	BIND(to_str, 1, 0);
	BIND(yeCreateString, 1, 2);
#include "binding.c"
#undef IN_CALL

	duk_pop(ctx);
	return 0;
}

static int loadString(void *s, const char *str)
{
	duk_eval_string(CTX(s), str);
	return 0;
}

static int loadFile(void *s, const char *file)
{
	int fd = open(file, O_RDONLY);
	yeAutoFree Entity *fstr = yeCreateString(NULL, NULL, NULL);
	struct stat st;

	if (stat(file, &st) < 0 || fd < 0) {
		DPRINT_ERR("cannot open/stat '%s'", file);
		return -1;
	}
	yeAddStrFromFd(fstr, fd, st.st_size);
	loadString(s, yeGetString(fstr));
	close(fd);
	return 0;
}

static void *allocator(void)
{
	YScriptDuk *ret;

	ret = g_new0(YScriptDuk, 1);
	ret->ops.init = init;
	ret->ops.destroy = destroy;
	ret->ops.loadFile = loadFile;
	ret->ops.loadString = loadString;
	ret->ops.call = call;
	return (void *)ret;
}

int ysDukInit(void)
{
	t = ysRegister(allocator);
	return t;
}

int ysDukEnd(void)
{
	return ysUnregiste(t);
}

int ysDukGetType(void)
{
	return t;
}
