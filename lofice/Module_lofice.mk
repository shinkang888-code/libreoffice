# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# lofice module — AI sidebar UI + integration library
#

$(eval $(call gb_Module_Module,lofice))

$(eval $(call gb_Module_add_targets,lofice,\
    Library_lofice \
    UIConfig_lofice \
))
