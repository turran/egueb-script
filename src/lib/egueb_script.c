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
#include "Egueb_Script.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static int _init = 0;
static Eina_Hash *_scripters = NULL;

typedef Egueb_Dom_Scripter * (*Egueb_Script_Ctor)(void);
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void egueb_script_init(void)
{
	if (!_init++)
	{
		egueb_dom_init();
		_scripters = eina_hash_string_superfast_new(NULL);
		/* register the scripters */
#if BUILD_JS_SM
		eina_hash_add(_scripters, "application/ecmascript",
				egueb_script_js_sm_scripter_new);
		eina_hash_add(_scripters, "text/ecmascript",
				egueb_script_js_sm_scripter_new);
		eina_hash_add(_scripters, "text/javascript",
				egueb_script_js_sm_scripter_new);
#endif
	}
}

EAPI void egueb_script_shutdown(void)
{
	if (_init == 1)
	{
		eina_hash_free(_scripters);
		egueb_dom_shutdown();
	}
	_init--;
}

EAPI Egueb_Dom_Scripter * egueb_script_scripter_new(const char *mime)
{
	Egueb_Script_Ctor ctor;

	ctor = eina_hash_find(_scripters, mime);
	if (ctor)
		return ctor();
	return NULL;
}
