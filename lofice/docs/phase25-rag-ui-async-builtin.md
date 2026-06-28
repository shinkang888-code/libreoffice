# Phase 25 — RAG UI 상태·비동기 내장 AI·퀵액션 자동 전송

## 요약

내장 AI 경로를 UI 스레드 동기 호출에서 **백그라운드 `AiAsyncPromptJob`** 으로 전환하고, RAG fetch 진행·결과 크기를 상태줄에 표시합니다. 퀵액션 AI 프롬프트 **자동 전송 ON/OFF** 토글을 officecfg·사이드바 설정에 추가했습니다.

## 변경 파일

| 영역 | 파일 |
|------|------|
| 비동기 프롬프트 job | `lofice/include/lofice/ai/AiAsyncPromptJob.hxx`, `lofice/source/ai/AiAsyncPromptJob.cxx` |
| RAG 상태 메시지 | `lofice/include/lofice/ai/AiRagContext.hxx`, `lofice/source/ai/AiRagContext.cxx` |
| 중복 RAG 방지 | `lofice/include/lofice/ai/AiPromptService.hxx` (`ragPrefetched`) |
| 스트리밍 RAG 상태 | `lofice/source/ai/AiAsyncStreamJob.cxx` |
| UI 패널 | `lofice/source/ui/AiAssistantPanel.hxx/cxx` |
| 설정 | `officecfg/.../Lofice.xcs`, `Lofice.xcu`, `AiSettingsStore.*` |
| UI 리소스 | `lofice/uiconfig/.../sidebar_ai_assistant.ui`, `LoficeSidebarLayout.hxx` |
| 빌드 | `lofice/Library_lofice.mk` |

## 동작

### 내장 AI (외부 API 미설정)

1. **전송** 클릭 → `trySendAsyncPrompt`
2. 상태: `코드베이스 검색 중...` → `RAG N자 — AI 처리 중...` (또는 `RAG 결과 없음`)
3. worker에서 `processPrompt` → 응답을 한 번에 표시
4. UI 프리즈 없음

### 외부 API 스트리밍

- RAG 활성 시 초기 상태 `코드베이스 검색 중...`
- fetch 후 `RAG N자 — AI 스트리밍 수신 중...`

### 퀵액션

- **AI 설정 → 퀵액션 AI 프롬프트 자동 전송** (기본 ON)
- OFF: UNO 실행 + 프롬프트 필드만 채움, 사용자가 **전송** 클릭

## officecfg

```
/org.openoffice.Office/Lofice/AiSettings/AutoSendQuickActionPrompt = true
```

## 빌드

Linux/WSL에서 `make lofice` (로컬 Windows Makefile 없음 — 기존과 동일).

## 검증 (Node/RAG — 빌드 불필요)

```bash
cd lofice/scripts
npm run verify-rag-e2e
```

## 다음 후보

1. API 키 rotation 후 `npm run sync-vercel`
2. git push + CI workflow
3. OptLoficeAiTabPage에 자동 전송 토글 추가
