# Phase 9 — 비동기 AI 스트리밍 (UI 비블로킹)

Phase 8의 동기 SSE 호출을 **백그라운드 스레드**로 옮기고, 토큰·완료 이벤트는 **Application::PostUserEvent** 로 메인 스레드에서 UI를 갱신합니다.

## 파이프라인

```
OnSendClicked
  → trySendExternalStreaming()
       → dispatchBefore (메인 스레드)
       → AiAsyncStreamJob::start()
            └─ std::thread
                 → postChatCompletionStreaming()
                      → WorkerSink::onStreamToken / onStreamFinished
                           → Application::PostUserEvent → OnAsyncStreamEvent
                                → appendResponseToken / handleStreamFinished
                                → dispatchAfter (Finished 시)
```

## 주요 파일

| 파일 | 역할 |
|------|------|
| `lofice/include/lofice/ai/AiAsyncStreamJob.hxx` | 백그라운드 작업 API |
| `lofice/source/ai/AiAsyncStreamJob.cxx` | std::thread + PostUserEvent |
| `lofice/source/ui/AiAssistantPanel.*` | `OnAsyncStreamEvent`, 스트리밍 중 버튼 비활성화 |

## 안전·수명

- `StreamUiAdapter::alive` — 패널 소멸 시 `false`, 보류 이벤트 무시
- `AiAsyncStreamJob::cancel()` — 소멸자·패널 파괴 시 스레드 join
- `SolarMutexGuard` — UI 갱신 시 SolarMutex 획득
- 진행 중 재전송 — 「이전 AI 요청이 진행 중입니다」 상태 표시

## 제한 (다음 단계 후보)

- ~~curl `perform` 중 즉시 취소(abort) 미지원~~ → **Phase 10**에서 XFERINFO abort
- ~~연결 테스트(`OnTestConnectionClicked`)는 여전히 동기 호출~~ → **Phase 10** 비동기화

## 빌드

```bash
make officecfg lofice cui sw sc sd
```
