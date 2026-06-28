# lofice 리브랜딩 가이드

## 기술 정책 (확정)

| 항목 | 결정 |
|------|------|
| 언어 | **모던 C++17/20 유지** — 순수 C 전환 **금지** |
| 아키텍처 | UNO + VCL/weld 객체 지향 유지 |
| Rust | 선택적 래핑만 — 코어 대체 아님 |

순수 C로의 전환은 재창조 수준이며 AI·빌드·UNO 호환 모두 붕괴 위험이 큽니다.

---

## 리브랜딩 3-Tier

| Tier | 스크립트 | 범위 | UNO ID 보호 |
|------|---------|------|------------|
| Display | `-Tier Display` | UI·설치·README 표시명 | ✅ |
| Headers | `-Tier Headers` | 파일 헤더 "lofice project" | ✅ |
| All | `-Tier All` | 전체 (라인 단위 UNO 보호) | ✅ |

```powershell
# 모듈별 실행 (권장 — 전체 한 번에 X)
.\lofice\scripts\rebrand-lofice.ps1 -Root .\scp2 -Tier Display
.\lofice\scripts\rebrand-lofice.ps1 -Root .\desktop -Tier Display
.\lofice\scripts\rebrand-lofice.ps1 -Root .\sysui -Tier Display

# 전체 리포 (시간 소요, 빌드 검증 필수)
.\lofice\scripts\rebrand-lofice.ps1 -Tier Display
```

---

## UNO 호환 — 변경하지 않는 것

다음은 **기능 유지**를 위해 코드 식별자로 잔존합니다:

- `org.libreoffice.*` — UNO 컴포넌트 등록
- `vnd.libreoffice.*` — 프로토콜 핸들러
- `com.sun.star.*` — UNO API (표준)

사용자에게 보이는 문자열만 **lofice** / **Lonex. Inc**로 교체합니다.

---

## 라이선스 (이중)

| 구분 | 파일 | 권리 |
|------|------|------|
| Upstream MPL | 각 `.cxx` MPL 헤더 | TDF/기여자 — **삭제 금지** |
| lofice 신규 | `lofice/**` | Lonex. Inc 독점 |
| 고지 | `lofice/LICENSE.md`, `COPYRIGHT-KR.md` | IP 실드 |

---

## 아이콘

```powershell
# 1. 마스터 로고: lofice/assets/lofice-logo-master.png (첨부 파일로 교체 가능)
# 2. 크기별 PNG 생성
.\lofice\scripts\generate-icons.ps1
# 3. icon-themes mainapp 슬롯 배치 (+ embed PNG)
.\lofice\scripts\deploy-icons.ps1
```

`deploy-icons.ps1`는 SVG와 함께 `lofice_16.png`, `lofice_32.png`를 각 테마 `res/`에 복사합니다.  
원본 mainapp 백업(`*.lofice.bak`)은 `.gitignore` 대상이며 커밋하지 않습니다.

---

## 진행 체크리스트 (2026-06-28)

| 단계 | 상태 | 내용 |
|------|------|------|
| 1. 아이콘 deploy | ✅ | 6 테마 mainapp + PNG |
| 2. 모듈 rebrand | ✅ | framework(360), sfx2(375), sw(3157), sc(2748), sd(1231) |
| 3. instsetoo/productlist | ✅ | productlist `lofice`, MSI ulf, archive 경로 |
| 4. CI 검증 | ✅ | `verify-rebrand`, workflow path 확장 |
| 5. MPL 문서 | ✅ | `lofice/docs/mpl-compliance.md` |
| 6. 추가 모듈 rebrand | ✅ | cui, vcl, officecfg, starmath, dbaccess, svx, chart2, xmloff, xmlsecurity, solenv 등 |
| Linux 바이너리 빌드 | ⏳ | self-hosted runner 필요 |

검증: `cd lofice/scripts && npm run verify-rebrand`

### Android 주의

`android/Bootstrap`, `android/source/src`의 Java 클래스명(`LibreOfficeKit` 등)은 JNI와 연동되므로 **rebrand 제외**합니다.  
UI 문자열은 `android/source/res`에만 Display tier를 적용하세요.

첨부 로고 파일을 `lofice/assets/lofice-logo-master.png`에 덮어쓴 뒤 스크립트를 재실행하세요.

---

## AI Hooking API

- 헤더: `lofice/include/lofice/IEventListener.hxx`
- 구현: `lofice/source/core/EventListenerRegistry.cxx`

Phase 2 파일 단위 이식 시 `EventListenerRegistry::dispatchBefore/After` 호출 지점을 추가합니다.

---

*Copyright (c) Lonex. Inc. All Rights Reserved.*
