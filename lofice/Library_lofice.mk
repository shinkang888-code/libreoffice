# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# lofice library — UI shell (AI sidebar, toolbar dispatch bridge)
#

$(eval $(call gb_Library_Library,lofice))

$(eval $(call gb_Library_set_include,lofice,\
    -I$(SRCDIR)/lofice/include \
    $$(INCLUDE) \
))

$(eval $(call gb_Library_use_api,lofice,\
    udkapi \
    offapi \
))

$(eval $(call gb_Library_use_custom_headers,lofice,\
    officecfg/registry \
))

$(eval $(call gb_Library_use_libraries,lofice,\
    comphelper \
    cppu \
    cppuhelper \
    sal \
    sfx \
    svl \
    tl \
    ucbhelper \
    vcl \
))

$(eval $(call gb_Library_use_externals,lofice,\
    boost_headers \
    $(if $(ENABLE_CURL),curl) \
))

$(eval $(call gb_Library_add_exception_objects,lofice,\
    lofice/source/ai/AiConversationHistory \
    lofice/source/ai/AiConversationJson \
    lofice/source/ai/AiConversationFileIo \
    lofice/source/ai/AiConversationStore \
    lofice/source/ai/AiAsyncHttpJob \
    lofice/source/ai/AiAsyncPromptJob \
    lofice/source/ai/AiAsyncRagJob \
    lofice/source/ai/AiAsyncStreamJob \
    lofice/source/ai/AiHttpConfig \
    lofice/source/ai/AiHttpTransport \
    lofice/source/ai/AiPromptService \
    lofice/source/ai/AiRagContext \
    lofice/source/ai/AiServiceBootstrap \
    lofice/source/ai/AiSettingsStore \
    lofice/source/ai/DefaultAiListener \
    lofice/source/core/EventListenerRegistry \
    lofice/source/ui/OptLoficeAiTabPage \
    lofice/source/ui/QuickActionCatalog \
    lofice/source/ui/ToolbarDispatchBridge \
    lofice/source/ui/AiAssistantPanel \
    lofice/source/ui/LoficePanelIntegration \
))
