/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */

/*

 * lofice — AI settings persistence (implementation)

 *

 * Copyright (c) Lonex. Inc. All Rights Reserved.

 *

 * This Source Code Form is subject to the terms of the Mozilla Public

 * License, v. 2.0. If a copy of the MPL was not distributed with this

 * file, You can obtain one at http://mozilla.org/MPL/2.0/.

 */



#include <lofice/ai/AiSettingsStore.hxx>



#include <memory>

#include <comphelper/configuration.hxx>
#include <officecfg/Office/Lofice.hxx>

#include <osl/process.h>

#include <rtl/ustring.hxx>



namespace lofice::ai

{



namespace

{



OUString readEnvOuString(std::string_view rName)

{

    const OUString aName = OUString::createFromAscii(rName);

    OUString aValue;

    if (osl_getEnvironment(aName.pData, &aValue.pData) != osl_Process_E_None)

        return {};

    return aValue;

}



bool readEnvBool(std::string_view rName)

{

    const OUString aValue = readEnvOuString(rName);

    if (aValue.isEmpty())

        return false;

    const OUString aLower = aValue.toAsciiLowerCase();

    return aLower == "1" || aLower == "true" || aLower == "yes" || aLower == "on";

}



sal_Int32 clampTimeout(sal_Int32 nValue)

{

    if (nValue < 5)

        return 5;

    if (nValue > 120)

        return 120;

    return nValue;

}



sal_Int32 clampRagTimeout(sal_Int32 nValue)

{

    if (nValue < 2)

        return 2;

    if (nValue > 30)

        return 30;

    return nValue;

}



void applyEnvFallback(AiSettings& rSettings)

{

    const OUString aRegistryEndpoint = officecfg::Office::Lofice::AiSettings::Endpoint::get();



    if (rSettings.endpoint.isEmpty())

        rSettings.endpoint = readEnvOuString("LOFICE_AI_ENDPOINT");

    if (rSettings.apiKey.isEmpty())

        rSettings.apiKey = readEnvOuString("LOFICE_AI_API_KEY");

    if (rSettings.model == u"lofice-default"_ustr)

    {

        const OUString aEnvModel = readEnvOuString("LOFICE_AI_MODEL");

        if (!aEnvModel.isEmpty())

            rSettings.model = aEnvModel;

    }

    if (rSettings.timeoutSeconds == 30)

    {

        const OUString aEnvTimeout = readEnvOuString("LOFICE_AI_TIMEOUT");

        if (!aEnvTimeout.isEmpty())

            rSettings.timeoutSeconds = clampTimeout(aEnvTimeout.toInt32());

    }



    if (!rSettings.endpoint.isEmpty() && rSettings.useBuiltinAi && aRegistryEndpoint.isEmpty())

        rSettings.useBuiltinAi = false;



    if (readEnvBool("LOFICE_RAG_ENABLED"))

        rSettings.useRagContext = true;



    const OUString aEnvRagEndpoint = readEnvOuString("LOFICE_RAG_ENDPOINT");

    if (!aEnvRagEndpoint.isEmpty())

        rSettings.ragEndpoint = aEnvRagEndpoint;



    const OUString aEnvRagTimeout = readEnvOuString("LOFICE_RAG_TIMEOUT");

    if (!aEnvRagTimeout.isEmpty())

        rSettings.ragTimeoutSeconds = clampRagTimeout(aEnvRagTimeout.toInt32());



    if (rSettings.ragEndpoint.isEmpty())

        rSettings.ragEndpoint = u"https://lofice-rag-api.vercel.app/search"_ustr;

}



} // namespace



AiSettings loadSettings()

{

    AiSettings aSettings;

    aSettings.useBuiltinAi = officecfg::Office::Lofice::AiSettings::UseBuiltinAi::get();

    aSettings.endpoint = officecfg::Office::Lofice::AiSettings::Endpoint::get();

    aSettings.apiKey = officecfg::Office::Lofice::AiSettings::ApiKey::get();

    aSettings.model = officecfg::Office::Lofice::AiSettings::Model::get();

    aSettings.timeoutSeconds

        = clampTimeout(officecfg::Office::Lofice::AiSettings::TimeoutSeconds::get());

    aSettings.useRagContext = officecfg::Office::Lofice::AiSettings::UseRagContext::get();

    aSettings.ragEndpoint = officecfg::Office::Lofice::AiSettings::RagEndpoint::get();

    aSettings.ragTimeoutSeconds = clampRagTimeout(

        officecfg::Office::Lofice::AiSettings::RagTimeoutSeconds::get());

    aSettings.autoSendQuickActionPrompt

        = officecfg::Office::Lofice::AiSettings::AutoSendQuickActionPrompt::get();

    applyEnvFallback(aSettings);

    return aSettings;

}



void saveSettings(const AiSettings& rSettings)

{

    std::shared_ptr<comphelper::ConfigurationChanges> xBatch(
        comphelper::ConfigurationChanges::create());

    officecfg::Office::Lofice::AiSettings::UseBuiltinAi::set(rSettings.useBuiltinAi, xBatch);

    officecfg::Office::Lofice::AiSettings::Endpoint::set(rSettings.endpoint, xBatch);

    officecfg::Office::Lofice::AiSettings::ApiKey::set(rSettings.apiKey, xBatch);

    officecfg::Office::Lofice::AiSettings::Model::set(rSettings.model, xBatch);

    officecfg::Office::Lofice::AiSettings::TimeoutSeconds::set(

        clampTimeout(rSettings.timeoutSeconds), xBatch);

    officecfg::Office::Lofice::AiSettings::UseRagContext::set(rSettings.useRagContext, xBatch);

    officecfg::Office::Lofice::AiSettings::RagEndpoint::set(rSettings.ragEndpoint, xBatch);

    officecfg::Office::Lofice::AiSettings::RagTimeoutSeconds::set(

        clampRagTimeout(rSettings.ragTimeoutSeconds), xBatch);

    officecfg::Office::Lofice::AiSettings::AutoSendQuickActionPrompt::set(

        rSettings.autoSendQuickActionPrompt, xBatch);

    xBatch->commit();

}



AiHttpConfig toHttpConfig(const AiSettings& rSettings)

{

    AiHttpConfig aConfig;

    if (rSettings.useBuiltinAi || rSettings.endpoint.isEmpty())

        return aConfig;



    aConfig.endpoint = rSettings.endpoint.toUtf8();

    aConfig.apiKey = rSettings.apiKey.toUtf8();

    aConfig.model = rSettings.model.toUtf8();

    aConfig.timeoutSeconds = rSettings.timeoutSeconds;

    return aConfig;

}



AiHttpConfig loadEffectiveHttpConfig()

{

    return toHttpConfig(loadSettings());

}



bool isExternalAiConfigured()

{

    const AiSettings aSettings = loadSettings();

    return !aSettings.useBuiltinAi && !aSettings.endpoint.isEmpty();

}



AiRagSettings toRagSettings(const AiSettings& rSettings)

{

    AiRagSettings aRag;

    aRag.useRagContext = rSettings.useRagContext;

    aRag.endpoint = rSettings.ragEndpoint.toUtf8();

    aRag.timeoutSeconds = rSettings.ragTimeoutSeconds;

    if (aRag.endpoint.empty())

        aRag.endpoint = "https://lofice-rag-api.vercel.app/search";

    return aRag;

}



} // namespace lofice::ai

