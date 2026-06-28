# Phase 10 — curl abort + 비동기 연결 테스트

Phase 9 이후 남은 HTTP 블로킹·취소 지연 문제를 해결합니다.

## 1. curl 즉시 abort

`performCurlRequest()`에 `CURLOPT_XFERINFOFUNCTION` 콜백을 연결하고, `std::atomic<bool>*` 취소 플래그가 `true`이면 `CURLE_ABORTED`로 전송을 중단합니다.

```cpp
postChatCompletion(..., pCancelFlag);
postChatCompletionStreaming(..., rSink, pCancelFlag);
```

- `AiAsyncStreamJob::cancel()` → `m_bCancelled = true` → curl 즉시 abort
- `WorkerSink::onStreamFinished` → UI에 `Finished` 이벤트 전달 (취소 시에도 UI 복구)

## 2. 비동기 연결 테스트

| 파일 | 역할 |
|------|------|
| `lofice/include/lofice/ai/AiAsyncHttpJob.hxx` | 비스트리밍 HTTP 백그라운드 작업 |
| `lofice/source/ai/AiAsyncHttpJob.cxx` | `postChatCompletion` worker + PostUserEvent |
| `AiAssistantPanel::OnAsyncHttpEvent` | 연결 테스트 결과 UI 반영 |

**연결 테스트** 버튼 클릭 시 UI 스레드를 차단하지 않고, 완료 후 응답 영역·상태 라벨을 갱신합니다.

## 파이프라인 (연결 테스트)

```
OnTestConnectionClicked
  → AiAsyncHttpJob::start()
       └─ std::thread → postChatCompletion(..., &m_bCancelled)
            → PostUserEvent → OnAsyncHttpEvent
```

## 빌드

```bash
make officecfg lofice cui sw sc sd
```

## 다음 단계 후보 (Phase 11)

- 스트리밍 중 **취소 버튼** UI
- 대화 히스토리 / 멀티턴 컨텍스트
- Linux CI 빌드 검증
