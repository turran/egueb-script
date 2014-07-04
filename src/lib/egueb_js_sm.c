/* Egueb Js SM - JavaScript support for Egueb (SpiderMonkey based)
 * Copyright (C) 2014 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#include "egueb_js_sm_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Egueb_Js_Sm_Scripter
{
	JSRuntime *rt;
	JSContext *cx;
	JSObject *global;
} Egueb_Js_Sm_Scripter;

static int _init = 0;

/* The class of the global object. */
static JSClass global_class =
{
	"global", JSCLASS_GLOBAL_FLAGS,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_StrictPropertyStub, JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub,
	JS_FinalizeStub, JSCLASS_NO_OPTIONAL_MEMBERS
};

static JSBool _egueb_js_sm_scripter_alert(JSContext *cx, uintN argc, jsval *vp)
{
	JSString* u16_txt;
	unsigned int length;
	char *txt;

	if (!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S", &u16_txt))
		return JS_FALSE;

	length = JS_GetStringEncodingLength(cx, u16_txt);
	txt = alloca(sizeof(char) * (length + 1));
	JS_EncodeStringToBuffer(u16_txt, txt, length);

	printf("%.*s\n", length, txt);
	return JS_TRUE;
}

static JSFunctionSpec global_functions[] =
{
    JS_FS("alert", _egueb_js_sm_scripter_alert, 1, 0),
    JS_FS_END
};

/* The error reporter callback. */
static void _egueb_js_sm_scripter_report_error(JSContext *cx,
		const char *message, JSErrorReport *report)
{
	Egueb_Js_Sm_Scripter *thiz;
	JSRuntime *rt;

	rt = JS_GetRuntime(cx);
	thiz = JS_GetRuntimePrivate(rt);
	
	fprintf(stderr, "%s:%u:%s\n",
			report->filename ? report->filename : "<no filename>",
			(unsigned int) report->lineno, message);
}

/*----------------------------------------------------------------------------*
 *                           Scripter descriptor                              *
 *----------------------------------------------------------------------------*/
static void * _egueb_js_sm_scripter_new(void)
{
	Egueb_Js_Sm_Scripter *thiz;

	thiz = calloc(1, sizeof(Egueb_Js_Sm_Scripter));
	/* Create an instance of the engine */
	thiz->rt = JS_NewRuntime(1024 * 1024);
	JS_SetRuntimePrivate(thiz->rt, thiz);

	/* Create an execution context */
	thiz->cx = JS_NewContext(thiz->rt, 8192);
	JS_SetOptions(thiz->cx, JS_GetOptions(thiz->cx) | JSOPTION_VAROBJFIX);
	JS_SetErrorReporter(thiz->cx, _egueb_js_sm_scripter_report_error);

	/* Create a global object and a set of standard objects */
	thiz->global = JS_NewCompartmentAndGlobalObject(thiz->cx, &global_class, NULL);
	/* TODO add the blobal dom functions: alert... */
	JS_DefineFunctions(thiz->cx, thiz->global, global_functions);
	/* Populate the global object with the standard globals,
	 * like Object and Array.
	 */
	JS_InitStandardClasses(thiz->cx, thiz->global);
	return thiz;
}

static void _egueb_js_sm_scripter_free(void *prv)
{
	Egueb_Js_Sm_Scripter *thiz = prv;

	JS_DestroyContext(thiz->cx);
	JS_DestroyRuntime(thiz->rt);
	free(thiz);
}

static Eina_Bool _egueb_js_sm_scripter_global_add(void *prv, Egueb_Dom_String *name, void *obj, const char *type)
{
#if 0
	JS_DefineObject(cx, global, "ender", ender_js_sm_class_get(), NULL, JSPROP_PERMANENT | JSPROP_READONLY);
#endif
}

static Eina_Bool _egueb_js_sm_scripter_load(void *prv, Egueb_Dom_String *s, void **obj)
{
	Egueb_Js_Sm_Scripter *thiz = prv;
	JSObject *script;
	const char *data;

	data = egueb_dom_string_string_get(s);
	script = JS_CompileScript(thiz->cx, NULL, data, strlen(data), NULL, 1);
	*obj = script;
	return script ? EINA_TRUE : EINA_FALSE;
}

static Eina_Bool _egueb_js_sm_scripter_run(void *prv, void *obj, Egueb_Dom_Node *ctx)
{
	Egueb_Js_Sm_Scripter *thiz = prv;
	JSObject *script = obj;
	jsval rval;
	JSBool ret;

	/* get the circle_click */
	ret = JS_ExecuteScript(thiz->cx, thiz->global, script, &rval);
	return ret;
}

static Egueb_Dom_Scripter_Descriptor _descriptor = {
	/* .new 	= */ _egueb_js_sm_scripter_new,
	/* .free 	= */ _egueb_js_sm_scripter_free,
	/* .load 	= */ _egueb_js_sm_scripter_load,
	/* .run 	= */ _egueb_js_sm_scripter_run,
	/* .global_add 	= */ _egueb_js_sm_scripter_global_add,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void egueb_js_sm_init(void)
{
	if (!_init++)
	{
		egueb_dom_init();
		ender_js_sm_init();
	}
}

EAPI void egueb_js_sm_shutdown(void)
{
	if (_init == 1)
	{
		ender_js_sm_shutdown();
		egueb_dom_shutdown();
	}
	_init--;
}

EAPI Egueb_Dom_Scripter * egueb_js_sm_scripter_new(void)
{
	Egueb_Dom_Scripter *ret;

	ret = egueb_dom_scripter_new(&_descriptor);
	return ret;
}
