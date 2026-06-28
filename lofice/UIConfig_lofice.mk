# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# lofice UI resources
#

$(eval $(call gb_UIConfig_UIConfig,modules/lofice))

$(eval $(call gb_UIConfig_add_uifiles,modules/lofice,\
    lofice/uiconfig/modules/lofice/ui/sidebar_ai_assistant \
    lofice/uiconfig/modules/lofice/ui/opt_lofice_ai_page \
))
