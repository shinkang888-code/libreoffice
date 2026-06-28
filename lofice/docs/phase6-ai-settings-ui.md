# Phase 6: AI 설정 UI

> **목표:** 환경 변수 없이 사이드바에서 API 설정 저장·테스트  
> **저장:** `officecfg` 사용자 레지스트리 (`org.openoffice.Office.Lofice`)

---

## 1. UI 위치

AI Assistant 사이드바 → **AI 설정** 버튼 → 설정 패널 펼침

| 필드 | 설명 |
|------|------|
| 내장 AI 사용 | 체크 시 외부 API 비활성 |
| API 엔드포인트 | Chat Completions URL |
| API 키 | Bearer 토큰 (마스킹 입력) |
| 모델 | JSON `model` 필드 |
| 타임아웃(초) | 5–120 |
| 설정 저장 | officecfg에 영구 저장 |
| 연결 테스트 | `ping` 프롬프트로 HTTP 검증 |

---

## 2. officecfg 등록

| 파일 | 역할 |
|------|------|
| `officecfg/registry/schema/org/openoffice/Office/Lofice.xcs` | 스키마 |
| `officecfg/registry/data/org/openoffice/Office/Lofice.xcu` | 기본값 |
| `officecfg/files.mk` | `Office/Lofice` 추가 |
| `officecfg/Configuration_officecfg.mk` | xcu 빌드 등록 |

C++ API: `officecfg::Office::Lofice::AiSettings::*`

---

## 3. lofice 모듈

| 파일 | 역할 |
|------|------|
| `lofice/include/lofice/ai/AiSettingsStore.hxx` | load/save/toHttpConfig |
| `lofice/source/ai/AiSettingsStore.cxx` | officecfg + env 폴백 |
| `lofice/source/ui/AiAssistantPanel.*` | 설정 UI 바인딩 |
| `lofice/uiconfig/.../sidebar_ai_assistant.ui` | grid_settings |

**우선순위:** UI 저장값 → env 변수(빈 필드만 보충)

---

## 4. 사용법

1. 사이드바 **AI Assistant** 덱 열기
2. **AI 설정** 클릭
3. 엔드포인트·API 키·모델 입력
4. **설정 저장** → **연결 테스트**
5. 프롬프트 입력 → **AI에 전송**

---

## 5. 빌드

```bash
make officecfg lofice sw sc sd
```

---

*Copyright (c) Lonex. Inc. All Rights Reserved.*
