# Figma Pro 연동 (lofice)

## 계정 상태

| 항목 | 값 |
|------|-----|
| 계정 | 신강 (`shinkang888@gmail.com`) |
| 플랜 | **Pro** (Expert seat) |
| 팀 | 신강의 팀 |
| planKey | `team::1610225395373160067` |
| Cursor MCP | `plugin-figma-figma` — **연동 완료** |

Cursor에서 Figma MCP `whoami` 호출 시 Pro 팀이 확인되면 연동이 정상입니다.

## 디자인 파일

**lofice — AI Sidebar & Design System**

- URL: https://www.figma.com/design/8P6zGFeW9XsqlOhHAXOm9R
- fileKey: `8P6zGFeW9XsqlOhHAXOm9R`

### 페이지

| 페이지 | 내용 |
|--------|------|
| Cover | 프로젝트 표지 |
| AI Sidebar | `AiAssistantPanel` 목업 (340px, 퀵액션 4×4, 프롬프트/응답) |
| Writer — Quick Actions | Writer 16슬롯 퀵액션 + UNO 명세표 (`QuickActionCatalog.cxx`) |
| Design Tokens | `lofice / Core` 변수 (color, spacing, radius, sidebar width) |

설정은 [`lofice/design/figma.config.json`](../design/figma.config.json)에 저장됩니다.

## Cursor에서 사용법

### 디자인 → 코드

Figma URL을 채팅에 붙여넣고 예:

```
이 Figma AI Sidebar를 lofice sidebar_ai_assistant.ui와 맞춰줘
https://www.figma.com/design/8P6zGFeW9XsqlOhHAXOm9R?node-id=1-6
```

에이전트가 `get_design_context` / `get_screenshot`으로 디자인을 읽고 C++ UI 리소스를 수정합니다.

### 코드 → 디자인

```
lofice AI 사이드바 설정 패널을 Figma에 반영해줘
```

에이전트가 `use_figma`로 Figma 파일을 업데이트합니다.

### 새 파일 생성

Pro 팀에 파일을 만들 때 `planKey`는 위 값을 사용합니다.

## Code Connect (선택)

| Figma 노드 | 코드 |
|------------|------|
| `AiAssistantPanel` | `lofice/source/ui/AiAssistantPanel.cxx` |
| GTK UI | `lofice/uiconfig/modules/lofice/ui/sidebar_ai_assistant.ui` |

Code Connect 매핑은 Cursor Figma MCP `add_code_connect_map`으로 확장할 수 있습니다.

## Pro 기능 활용

- **팀 라이브러리**: `get_libraries` + `search_design_system`으로 공유 컴포넌트 검색
- **Variables**: Design Tokens 페이지 — VCL/GTK 스타일과 동기화 가능
- **Dev Mode**: fileKey + node-id로 스펙 handoff

## 문제 해결

| 증상 | 조치 |
|------|------|
| MCP 권한 오류 | Cursor **Settings → MCP → Figma** 재로그인 |
| Pro 팀 파일 접근 불가 | `whoami`로 planKey 확인, 파일을 해당 팀 Drafts/Project에 생성 |
| node-id 형식 | URL `1-6` → MCP `1:6` |
