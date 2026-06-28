# Phase 7: 도구 → 옵션 AI 설정 페이지

> **목표:** 사이드바와 동일한 AI 설정을 **도구 → 옵션**에서 편집  
> **저장:** `AiSettingsStore` → `officecfg::Office::Lofice::AiSettings`

---

## 1. 위치

**도구 → 옵션 → lofice (또는 %PRODUCTNAME%) → AI 어시스턴트**

사이드바 **AI 설정** 패널과 **동일한 officecfg**를 읽고 씁니다.

---

## 2. 파일

| 파일 | 역할 |
|------|------|
| `lofice/include/lofice/ui/OptLoficeAiTabPage.hxx` | SfxTabPage 선언 |
| `lofice/source/ui/OptLoficeAiTabPage.cxx` | Reset/FillItemSet, 연결 테스트 |
| `lofice/uiconfig/.../opt_lofice_ai_page.ui` | 옵션 UI |
| `include/sfx2/pageids.hxx` | `RID_SVXPAGE_LOFICE_AI_OPTIONS` |
| `cui/inc/treeopt.hrc` | 옵션 트리 등록 |
| `cui/source/options/treeopt.cxx` | Create 팩토리 |
| `cui/Library_cui.mk` | `lofice` 링크 |

---

## 3. 필드

- 내장 AI 사용
- API 엔드포인트 / API 키 / 모델 / 타임아웃
- **연결 테스트** (성공/실패 메시지 박스)

**확인** 또는 **적용** 시 `saveSettings()` 호출.

---

## 4. 빌드

```bash
make officecfg lofice cui sw sc sd
```

---

*Copyright (c) Lonex. Inc. All Rights Reserved.*
