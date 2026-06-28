# Figma ↔ Code 동기화 (Phase 26)

## 디자인 → 코드 (node-id=1:6)

Figma `AiAssistantPanel` 스펙을 [`sidebar_ai_assistant.ui`](../uiconfig/modules/lofice/ui/sidebar_ai_assistant.ui) 및 [`AiAssistantPanel.cxx`](../source/ui/AiAssistantPanel.cxx)에 반영.

| Figma | 코드 변경 |
|-------|-----------|
| padding 12px, gap 8px | `margin=12`, `row-spacing=8` |
| 캡션 라벨 없음 | `label_prompt_caption`, `label_response_caption` 제거 |
| 순서: 프롬pt → 응답 → 상태 → 액션 | `top-attach` 재배치 |
| 프롬pt 80px / 응답 160px | `min-content-height`, `set_size_request` |
| 버튼 `전송` · `취소` · `지우기` | `btn_send`, `btn_cancel_async`, `btn_clear_prompt` |
| 상태 `준비됨 — 16개 퀵액션, RAG 활성` | `label_status` 기본값 + RAG 설정 시 cxx |

## 코드 → 디자인 (grid_settings)

[`sidebar_ai_assistant.ui`](../uiconfig/modules/lofice/ui/sidebar_ai_assistant.ui) `grid_settings` 전체를 Figma **`grid_settings / Expanded`** (`6:2`) 프레임으로 추가.

포함 필드: 내장 AI, API, RAG, 자동 전송, 저장/테스트 버튼.

## 링크

- 패널: https://www.figma.com/design/8P6zGFeW9XsqlOhHAXOm9R?node-id=1-6
- 설정: https://www.figma.com/design/8P6zGFeW9XsqlOhHAXOm9R?node-id=6-2

## 재동기화 명령 (Cursor)

```
디자인 → 코드: Figma URL + node-id=1-6 붙여넣고 "UI 맞춰줘"
코드 → 디자인: "사이드바 설정 패널 Figma에 반영해줘"
```
