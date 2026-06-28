# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the lofice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_Library_Library,loficekitgtk))

$(eval $(call gb_Library_use_sdk_api,loficekitgtk))

$(eval $(call gb_Library_add_exception_objects,loficekitgtk,\
    libreofficekit/source/gtk/lokdocview \
    libreofficekit/source/gtk/tilebuffer \
))

$(eval $(call gb_Library_use_externals,loficekitgtk,\
    boost_headers \
))

$(eval $(call gb_Library_set_include,loficekitgtk,\
    $$(INCLUDE) \
    $$(GTK3_CFLAGS) \
))

$(eval $(call gb_Library_add_libs,loficekitgtk,\
    $(GTK3_LIBS) \
))

$(eval $(call gb_Library_add_defs,loficekitgtk,\
	-DLOK_PATH="\"$(LIBDIR)/lofice/$(LIBO_LIB_FOLDER)\"" \
	-DLOK_DOC_VIEW_IMPLEMENTATION \
))

ifeq ($(OS),$(filter LINUX %BSD SOLARIS, $(OS)))
$(eval $(call gb_Library_add_libs,loficekitgtk,\
    $(UNIX_DLAPI_LIBS) -lm \
))
endif

$(eval $(call gb_Library_use_packages,loficekitgtk, \
    loficekit_selectionhandles \
))

# vim: set noet sw=4 ts=4:
