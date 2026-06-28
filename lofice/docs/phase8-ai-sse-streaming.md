# Phase 8 — AI SSE 스트리밍

외부 OpenAI 호환 API 응답을 **Server-Sent Events(SSE)** 로 수신해 사이드바 응답 영역에 토큰 단위로 표시합니다.

## 파이프라인

```
Send to AI (OnSendClicked)
  ├─ 외부 API 설정 + curl 사용 가능
  │    → trySendExternalStreaming()
  │         → postChatCompletionStreaming()
  │              → SseStreamParser (data: … delta.content)
  │                   → StreamingUiSink::onStreamToken()
  │                        → appendResponseToken()  (UI 갱신)
  │                   → onStreamFinished()
  └─ 그 외 (내장 AI / curl 없음)
       → fireAiHookEvent → DefaultAiListener → processPrompt (일괄 응답)
```

## 주요 파일

| 파일 | 역할 |
|------|------|
| `lofice/include/lofice/ai/AiStreamSink.hxx` | 스트리밍 콜백 인터페이스 |
| `lofice/include/lofice/ai/AiHttpTransport.hxx` | `postChatCompletionStreaming()` 선언 |
| `lofice/source/ai/AiHttpTransport.cxx` | `"stream":true`, SSE `data:` 파싱 |
| `lofice/source/ui/AiAssistantPanel.*` | `StreamingUiSink`, `appendResponseToken()` |

## 동작 요약

- **외부 API**가 활성(`UseBuiltinAi=false`, 엔드포인트 설정)이고 curl 빌드가 있으면 스트리밍 경로를 사용합니다.
- `DefaultAiListener`의 동기 `postChatCompletion`은 **내장 AI** 및 **curl 미사용** 시에만 호출됩니다.
- `dispatchBefore` / `dispatchAfter` 훅은 스트리밍·내장 경로 모두에서 유지됩니다.

## HTTP 요청

- JSON body에 `"stream": true`
- `Accept: text/event-stream`
- OpenAI Chat Completions 형식: `choices[].delta.content`

## 제한

- ~~curl `perform`은 UI 스레드에서 동기 실행~~ → **Phase 9**에서 백그라운드 스레드로 이동
- curl abort 즉시 취소 미지원 (Phase 10 후보)

## 빌드

```bash
make officecfg lofice cui sw sc sd
```

`ENABLE_CURL=YES` 필요.
