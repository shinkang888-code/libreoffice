/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
/*
 * lofice — AI prompt service (implementation)
 *
 * Copyright (c) Lonex. Inc. All Rights Reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <lofice/ai/AiPromptService.hxx>

#include <lofice/ai/AiHttpTransport.hxx>
#include <lofice/ai/AiRagContext.hxx>
#include <lofice/ai/AiSettingsStore.hxx>

#include <sstream>

namespace lofice::ai
{

namespace
{

std::string buildBuiltinResponse(const AiPromptRequest& rRequest)
{
    std::ostringstream aOut;
    aOut << "[lofice AI — 내장]\n";
    aOut << "컨텍스트: " << rRequest.appDisplayName << " (" << rRequest.appModule << ")\n\n";

    const std::string& rPrompt = rRequest.prompt;
    if (rPrompt.find("bold") != std::string::npos || rPrompt.find("Bold") != std::string::npos
        || rPrompt.find("굵") != std::string::npos)
    {
        aOut << "팁: Bold 퀵액션 또는 .uno:Bold 를 사용하세요.\n";
    }
    if (rPrompt.find("undo") != std::string::npos || rPrompt.find("Undo") != std::string::npos
        || rPrompt.find("실행 취소") != std::string::npos)
    {
        aOut << "팁: Undo 퀵액션 (.uno:Undo) 을 사용하세요.\n";
    }
    if (rRequest.appModule == "sc"
        && (rPrompt.find("sum") != std::string::npos || rPrompt.find("합") != std::string::npos))
    {
        aOut << "팁: Calc에서 AutoSum (.uno:AutoSum) 퀵액션을 사용하세요.\n";
    }
    if (rRequest.appModule == "sd"
        && (rPrompt.find("slide") != std::string::npos || rPrompt.find("슬라이드") != std::string::npos))
    {
        aOut << "팁: 새 슬라이드 (.uno:InsertPageQuick) 퀵액션을 사용하세요.\n";
    }

    aOut << "\n프롬프트 (" << rPrompt.size() << "자):\n" << rPrompt;

    if (!rRequest.ragContext.empty())
        aOut << "\n\n[코드베이스 RAG 컨텍스트]\n" << rRequest.ragContext;

    aOut << "\n\n외부 AI: 사이드바 'AI 설정'에서 API를 연결하세요.";
    return aOut.str();
}

std::string buildHttpErrorResponse(const AiPromptRequest& rRequest, const AiHttpResponse& rHttp)
{
    std::ostringstream aOut;
    aOut << "[lofice AI — HTTP 오류]\n";
    aOut << "상태: " << rHttp.httpStatus << "\n";
    aOut << "내용: " << rHttp.errorDetail << "\n\n";
    aOut << "프롬프트:\n" << rRequest.prompt;
    return aOut.str();
}

} // namespace

bool isExternalEndpointConfigured()
{
    return isExternalAiConfigured();
}

AiPromptResult processPrompt(const AiPromptRequest& rRequest)
{
    AiPromptResult aResult;
    if (rRequest.prompt.empty())
    {
        aResult.success = false;
        aResult.response = "프롬프트가 비어 있습니다.";
        aResult.provider = "builtin";
        return aResult;
    }

    const AiHttpConfig aHttpConfig = loadEffectiveHttpConfig();
    if (aHttpConfig.isValid())
    {
        if (!isHttpTransportAvailable())
        {
            aResult.success = false;
            aResult.response = "외부 AI가 설정되었으나 curl 빌드가 비활성화되어 있습니다.";
            aResult.provider = "http-unavailable";
            return aResult;
        }

        const AiPromptRequest aEnriched = [&]() {
            AiPromptRequest aCopy = rRequest;
            if (!aCopy.ragPrefetched)
                enrichPromptWithRag(aCopy);
            return aCopy;
        }();

        const AiHttpResponse aHttp = postChatCompletion(aHttpConfig, aEnriched);
        if (aHttp.success)
        {
            aResult.success = true;
            aResult.response = aHttp.content;
            aResult.provider = "http";
            return aResult;
        }

        aResult.success = true;
        aResult.response = buildHttpErrorResponse(rRequest, aHttp);
        aResult.provider = "http-error";
        return aResult;
    }

    AiPromptRequest aEnriched = rRequest;
    if (!aEnriched.ragPrefetched)
        enrichPromptWithRag(aEnriched);

    aResult.success = true;
    aResult.response = buildBuiltinResponse(aEnriched);
    aResult.provider = "builtin";
    return aResult;
}

} // namespace lofice::ai
