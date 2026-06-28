# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the lofice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_Executable_Executable,gtktiledviewer))

$(eval $(call gb_Library_use_sdk_api,gtktiledviewer))

$(eval $(call gb_Executable_set_include,gtktiledviewer,\
    $$(INCLUDE) \
    -I$(SRCDIR)/desktop/inc \
    -I$(SRCDIR)/loficekit/qa/gtktiledviewer/ \
    -I$(WORKDIR)/UnoApiHeadersTarget/offapi/normal/ \
    -I$(WORKDIR)/UnoApiHeadersTarget/udkapi/normal/ \
))

$(eval $(call gb_Executable_use_externals,gtktiledviewer,\
    boost_headers \
))

$(eval $(call gb_Executable_add_cxxflags,gtktiledviewer,\
    $$(GTK3_CFLAGS) \
))

$(eval $(call gb_Executable_add_libs,gtktiledviewer,\
    $(GTK3_LIBS) \
))

ifneq ($(OS), WNT)
$(eval $(call gb_Executable_add_libs,gtktiledviewer,\
    -lX11 \
    -lXext \
    -lXrender \
    -lSM \
    -lICE \
))
endif

$(eval $(call gb_Executable_use_libraries,gtktiledviewer,\
    loficekitgtk \
))

ifeq ($(OS), $(filter LINUX %BSD SOLARIS, $(OS)))
$(eval $(call gb_Executable_add_libs,gtktiledviewer,\
    -lm $(UNIX_DLAPI_LIBS) \
))
endif

$(eval $(call gb_Executable_add_exception_objects,gtktiledviewer,\
    loficekit/qa/gtktiledviewer/gtv-main \
    loficekit/qa/gtktiledviewer/gtv-application \
    loficekit/qa/gtktiledviewer/gtv-application-window \
    loficekit/qa/gtktiledviewer/gtv-main-toolbar \
    loficekit/qa/gtktiledviewer/gtv-signal-handlers \
    loficekit/qa/gtktiledviewer/gtv-helpers \
    loficekit/qa/gtktiledviewer/gtv-lokdocview-signal-handlers \
    loficekit/qa/gtktiledviewer/gtv-calc-header-bar \
    loficekit/qa/gtktiledviewer/gtv-comments-sidebar \
    loficekit/qa/gtktiledviewer/gtv-lok-dialog \
))

# vim: set noet sw=4 ts=4:
