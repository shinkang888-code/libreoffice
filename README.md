# lofice

**AI 친화형 모던 오피스 스위트** — LibreOffice 코어 엔진 기반, Lonex. Inc 제작

| 항목 | 내용 |
|------|------|
| 권리자 | Lonex. Inc — shinkang888@gmail.com |
| 발명자 | kangjunchul8@gmail.com |
| 저작권 | 대한민국 저작권 등록 프로그램 |
| 기술 | 모던 C++17/20 + UNO (순수 C 전환 없음) |

---

## 개요

lofice는 통합 오피스 스uite로, Writer / Calc / Impress / Draw / Base / Math를 포함합니다.

- **코어:** 객체 지향 C++ + UNO 컴포넌트 모델 (LibreOffice 기반 파생)
- **UI:** VCL / weld 추상화
- **AI:** 이벤트 리스너 Hooking API (`lofice::IEventListener`)

---

## 주요 모듈

| 모듈 | 설명 |
|------|------|
| [sal/](sal) | System Abstraction Layer |
| [vcl/](vcl) | 위젯 툴킷 + 렌더링 |
| [framework/](framework) | UNO UI chrome (툴바/메뉴) |
| [sfx2/](sfx2) | 문서/뷰/슬롯 프레임워크 |
| [sw/](sw/) | Writer |
| [sc/](sc/) | Calc |
| [sd/](sd/) | Draw / Impress |
| [lofice/](lofice/) | 브랜딩·라이선스·AI Hooking·문서 |

---

## 라이선스

- **Upstream (LibreOffice 기반):** Mozilla Public License 2.0 — [LICENSE](COPYING.MPL)
- **lofice 신규 추가분:** Lonex. Inc 독점 — [lofice/LICENSE.md](lofice/LICENSE.md)
- **저작권 고지:** [lofice/COPYRIGHT-KR.md](lofice/COPYRIGHT-KR.md)

---

## 리브랜딩

```powershell
# 사용자 표시 문자열 일괄 치환 (UNO 서비스 ID 보호)
.\lofice\scripts\rebrand-lofice.ps1 -Tier Display

# 아이콘 생성 및 배치
.\lofice\scripts\generate-icons.ps1
.\lofice\scripts\deploy-icons.ps1
```

---

## 문서

- [Phase 1 아키텍처 매핑](lofice/docs/phase1-architecture-mapping.md)

---

*Copyright (c) Lonex. Inc. All Rights Reserved.*
