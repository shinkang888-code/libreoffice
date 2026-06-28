# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#*************************************************************************
#
# This file is part of the lofice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
#*************************************************************************

$(eval $(call gb_CppunitTest_CppunitTest,loficekit_tiledrendering))

$(eval $(call gb_CppunitTest_add_exception_objects,loficekit_tiledrendering, \
    libreofficekit/qa/unit/tiledrendering \
))

$(eval $(call gb_CppunitTest_use_external,loficekit_tiledrendering,boost_headers))

# We need all these libraries / etc. due for CppunitTest to work, even though
# our test specifically tests LOK only functionality which would otherwise not
# require any normal LO api/libraries.
$(eval $(call gb_CppunitTest_use_libraries,loficekit_tiledrendering, \
	sal \
))

ifeq ($(OS),LINUX)
$(eval $(call gb_CppunitTest_add_libs,loficekit_tiledrendering,\
    -lm \
    -ldl \
))
endif

$(eval $(call gb_CppunitTest_use_api,loficekit_tiledrendering,\
    offapi \
))

$(eval $(call gb_CppunitTest_use_configuration,loficekit_tiledrendering))

# vim: set noet sw=4 ts=4:
