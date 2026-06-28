# Phase 21 — Quick Action AI 프롬프트 + 대화 JSON export/import

## 요약

퀵액션 클릭 시 UNO 명령 실행과 함께 AI 프롬프트를 자동 입력하고, 사이드바에서 대화 기록을 JSON 파일로 내보내기/가져오기할 수 있습니다. RAG 기본 URL은 Vercel Production으로 갱신했습니다.

## Quick Action → AI 프롬프트

| 구성 | 경로 |
|------|------|
| 카탈로그 | `lofice/include/lofice/ui/QuickActionCatalog.hxx` |
| 앱별 16슬롯 + `aiPrompt` | `lofice/source/ui/QuickActionCatalog.cxx` |
| UI 연동 | `AiAssistantPanel::OnQuickActionClicked` |

동작:

1. 퀵액션 버튼 클릭
2. `.uno:*` 명령 디스패치 (기존)
3. `aiPrompt`가 있으면 `text_prompt`에 UTF-8 한국어 프롬프트 자동 입력
4. 상태: `실행됨 + AI 프롬프트 입력: .uno:...`

## 대화 JSON export/import

| 구성 | 경로 |
|------|------|
| 직렬화 | `lofice/source/ai/AiConversationJson.cxx` |
| 파일 I/O | `lofice/source/ai/AiConversationFileIo.cxx` |
| officecfg | `AiConversationStore` → `AiConversationJson` 위임 |
| UI 버튼 | `btn_export_history`, `btn_import_history` |

### 파일 형식

```json
{
  "format": "lofice-chat-history",
  "version": 1,
  "exportedAt": "2026-06-28T12:00:00Z",
  "messages": [
    { "role": "user", "content": "..." },
    { "role": "assistant", "content": "..." }
  ]
}
```

가져오기는 위 envelope 또는 officecfg와 동일한 `[{role,content},...]` 배열도 지원합니다.

## RAG 기본 URL

- officecfg: `https://lofice-rag-api.vercel.app/search`
- 사이드바 UI 기본값 동일
- `AiSettingsStore` / `AiAssistantPanel` 폴백 동일

로컬 RAG 서버 사용 시 사이드바 또는 `LOFICE_RAG_ENDPOINT`로 덮어쓰면 됩니다.

## 검증

```bash
npm run verify-ci   # 20 sources, export/import UI 위젯
npm run verify-mk
```

## 다음 단계

- `.env.local`에 `OPENAI_API_KEY` 추가 → `npm run embed` → semantic 검색
- self-hosted runner 등록 후 Linux full build
