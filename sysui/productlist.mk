# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the lofice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

PRODUCTLIST := lofice loficeDev
PKGVERSION := $(LIBO_VERSION_MAJOR).$(LIBO_VERSION_MINOR).$(LIBO_VERSION_MICRO)
PKGVERSIONSHORT := $(LIBO_VERSION_MAJOR).$(LIBO_VERSION_MINOR)
PRODUCTNAME.lofice := lofice
PRODUCTNAME.loficeDev := loficeDev
UNIXFILENAME.lofice := lofice$(PKGVERSIONSHORT)
UNIXFILENAME.loficeDev := loficeDev$(PKGVERSIONSHORT)

# vim: set noet sw=4 ts=4:
