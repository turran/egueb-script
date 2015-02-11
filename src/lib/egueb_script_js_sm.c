/* Egueb Script - Scripting support for Egueb
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
#include "egueb_script_private.h"

#include <jsapi.h>
#include <Ender_Js_Sm.h>
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Egueb_Script_Js_Sm_Scripter
{
	JSRuntime *rt;
	JSContext *cx;
	JSObject *global;
} Egueb_Script_Js_Sm_Scripter;

typedef struct _Egueb_Script_Js_Sm_Scripter_Script
{
	JSObject *obj;
	JSObject *ctx;
} Egueb_Script_Js_Sm_Scripter_Script;

static int _init = 0;

/* The class of the global object. */
static JSClass global_class =
{
	"global", JSCLASS_GLOBAL_FLAGS,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_StrictPropertyStub, JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub,
	JS_FinalizeStub, JSCLASS_NO_OPTIONAL_MEMBERS
};

static JSBool _egueb_script_js_sm_scripter_alert(JSContext *cx, uintN argc, jsval *vp)
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
	JS_FS("alert", _egueb_script_js_sm_scripter_alert, 1, 0),
	JS_FS_END
};

/* The error reporter callback. */
static void _egueb_script_js_sm_scripter_report_error(JSContext *cx,
		const char *message, JSErrorReport *report)
{
	fprintf(stderr, "%s:%u:%s\n",
			report->filename ? report->filename : "<no filename>",
			(unsigned int) report->lineno, message);
}

static void _egueb_script_js_sm_init_global_object(Egueb_Script_Js_Sm_Scripter *thiz)
{
	/* Create a global object and a set of standard objects */
	thiz->global = JS_NewCompartmentAndGlobalObject(thiz->cx, &global_class, NULL);
	/* TODO add the global dom functions: alert... */
	JS_DefineFunctions(thiz->cx, thiz->global, global_functions);
	/* Populate the global object with the standard globals,
	 * like Object and Array.
	 */
	JS_InitStandardClasses(thiz->cx, thiz->global);
}

/*----------------------------------------------------------------------------*
 *                           Scripter descriptor                              *
 *----------------------------------------------------------------------------*/
static void * _egueb_script_js_sm_scripter_new(void)
{
	Egueb_Script_Js_Sm_Scripter *thiz;

	thiz = calloc(1, sizeof(Egueb_Script_Js_Sm_Scripter));
	/* Create an instance of the engine */
	thiz->rt = JS_NewRuntime(1024 * 1024);
	JS_SetRuntimePrivate(thiz->rt, thiz);

	/* Create an execution context */
	thiz->cx = JS_NewContext(thiz->rt, 8192);
	JS_SetOptions(thiz->cx, JS_GetOptions(thiz->cx) | JSOPTION_VAROBJFIX);
	JS_SetErrorReporter(thiz->cx, _egueb_script_js_sm_scripter_report_error);

	_egueb_script_js_sm_init_global_object(thiz);
	return thiz;
}

static void _egueb_script_js_sm_scripter_free(void *prv)
{
	Egueb_Script_Js_Sm_Scripter *thiz = prv;

	JS_DestroyContext(thiz->cx);
	JS_DestroyRuntime(thiz->rt);
	free(thiz);
}

static void _egueb_script_js_sm_scripter_global_add(void *prv, const char *name, void *obj, Ender_Item *i)
{
	Egueb_Script_Js_Sm_Scripter *thiz = prv;
	JSObject *jsobj;

	jsobj = ender_js_sm_instance_new(thiz->cx, i, obj);
	JS_DefineProperty(thiz->cx, thiz->global, name, OBJECT_TO_JSVAL(jsobj),
			NULL, NULL, JSPROP_READONLY);
}

static void _egueb_script_js_sm_scripter_global_clear(void *prv)
{
	Egueb_Script_Js_Sm_Scripter *thiz = prv;

	_egueb_script_js_sm_init_global_object(thiz);
}

static Eina_Bool _egueb_script_js_sm_scripter_load(void *prv, Egueb_Dom_String *s, void **obj)
{
	Egueb_Script_Js_Sm_Scripter *thiz = prv;
	Egueb_Dom_String *uri = NULL;
	jsval val;
	JSObject *so;
	Eina_Bool ret = EINA_FALSE;
	const char *data;

	*obj = NULL;
	data = egueb_dom_string_string_get(s);
	/* get the document, get the uri of the document, pass it as the filename */
	if (JS_GetProperty(thiz->cx, thiz->global, "document", &val))
	{
		JSObject *doc;

		doc = JSVAL_TO_OBJECT(val);
		if (ender_js_sm_is_instance(thiz->cx, doc))
		{
			Egueb_Dom_Node *n;
			n = ender_js_sm_instance_ptr_get(thiz->cx, doc);
			uri = egueb_dom_document_uri_get(n);
		}
	}
	so = JS_CompileScript(thiz->cx, NULL, data, strlen(data),
			uri ? egueb_dom_string_string_get(uri) : NULL, 1);
	if (so)
	{
		Egueb_Script_Js_Sm_Scripter_Script *script;
		script = calloc(1, sizeof(Egueb_Script_Js_Sm_Scripter_Script));
		script->obj = so;
		*obj = script;
		ret = EINA_TRUE;
	}
	if (uri)
	{
		egueb_dom_string_unref(uri);
	}

	return ret;
}

static Eina_Bool _egueb_script_js_sm_scripter_script_run(void *prv, void *s)
{
	Egueb_Script_Js_Sm_Scripter *thiz = prv;
	Egueb_Script_Js_Sm_Scripter_Script *script = s;
	jsval rval;
	JSBool ret;

	/* get the circle_click */
	ret = JS_ExecuteScript(thiz->cx, thiz->global, script->obj, &rval);
	return ret;
}

static Eina_Bool _egueb_script_js_sm_scripter_script_run_listener(void *prv, void *s, Egueb_Dom_Event *ev)
{
	Egueb_Script_Js_Sm_Scripter *thiz = prv;
	Egueb_Script_Js_Sm_Scripter_Script *script = s;
	JSObject *evt;
	jsval rval;
	JSBool ret;
	Ender_Item *i;

	/* TODO create 'this' in case it does not exist yet */
	/* set 'evt' as part of global */
	i = egueb_dom_event_item_get(ev);
	evt = ender_js_sm_instance_new(thiz->cx, i, egueb_dom_event_ref(ev));

	JS_DefineProperty(thiz->cx, thiz->global, "evt",
			OBJECT_TO_JSVAL(evt), NULL, NULL,
			JSPROP_READONLY);

	ret = JS_ExecuteScript(thiz->cx, thiz->global, script->obj, &rval);
	/* remove the evt */
	JS_DeleteProperty(thiz->cx, thiz->global, "evt");

	return ret;
}

static void _egueb_script_js_sm_scripter_script_free(void *prv, void *s)
{
	Egueb_Script_Js_Sm_Scripter_Script *script = s;

	script->obj = NULL;
	script->ctx = NULL;
	free(script);
}

static Egueb_Dom_Scripter_Descriptor _descriptor = {
	/* .version 		= */ EGUEB_DOM_SCRIPTER_DESCRIPTOR_VERSION,
	/* .new 		= */ _egueb_script_js_sm_scripter_new,
	/* .free 		= */ _egueb_script_js_sm_scripter_free,
	/* .load 		= */ _egueb_script_js_sm_scripter_load,
	/* .global_add 		= */ _egueb_script_js_sm_scripter_global_add,
	/* .global_clear 	= */ _egueb_script_js_sm_scripter_global_clear,
	/* .script_free 	= */ _egueb_script_js_sm_scripter_script_free,
	/* .script_run 		= */ _egueb_script_js_sm_scripter_script_run,
	/* .script_run_listener = */ _egueb_script_js_sm_scripter_script_run_listener,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Egueb_Dom_Scripter * egueb_script_js_sm_scripter_new(void)
{
#if BUILD_JS_SM
	Egueb_Dom_Scripter *ret;

	if (!_init++)
		ender_js_sm_init();

	ret = egueb_dom_scripter_new(&_descriptor);
	return ret;
#else
	return NULL;
#endif
}
