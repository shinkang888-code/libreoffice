# Phase 13 — 대화 기록 officecfg 영속화

Phase 12 in-memory 멀티턴 대화를 **사용자 officecfg**에 JSON으로 저장합니다. 앱 재시작 후에도 API 컨텍스트가 유지됩니다.

## 저장 위치

| 항목 | 값 |
|------|-----|
| 스키마 | `officecfg/.../Lofice.xcs` → `AiSettings/ChatHistoryJson` |
| 기본값 | `Lofice.xcu` 빈 문자열 |
| 런타임 | `officecfg::Office::Lofice::AiSettings::ChatHistoryJson` |

## JSON 형식

```json
[
  {"role":"user","content":"..."},
  {"role":"assistant","content":"..."}
]
```

최대 **20 messages** (AiConversationHistory) + **64KB** persist 상한.

## API

| 함수 | 시점 |
|------|------|
| `loadConversationHistory()` | 패널 생성 시 |
| `saveConversationHistory()` | 턴 commit / 응답·기록 지우기 |

## 주요 파일

- `lofice/include/lofice/ai/AiConversationStore.hxx`
- `lofice/source/ai/AiConversationStore.cxx`
- `officecfg/registry/schema/.../Lofice.xcs`
- `officecfg/registry/data/.../Lofice.xcu`

## 빌드

officecfg 재생성 필요:

```bash
make officecfg lofice cui sw sc sd
```

## 다음 단계 후보 (Phase 14)

- pgvector OpenAI embedding 시맨틱 코드 검색
- 마지막 assistant 응답 UI 복원
