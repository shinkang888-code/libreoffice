# lofice MPL 2.0 컴플라이언스 가이드

Copyright (c) Lonex. Inc. All Rights Reserved.

lofice는 lofice(MPL 2.0) 기반 **파생 프로그램(Derivative Work)** 입니다.  
본 문서는 배포·소스 공개·브랜딩 분리 시 준수해야 할 MPL 의무와 Lonex. Inc 독점 영역을 정리합니다.

---

## 1. 이중 라이선스 구조

| 영역 | 라이선스 | 소스 공개 |
|------|----------|-----------|
| Upstream MPL 파일 (`.cxx` 등 MPL 헤더 보유) | MPL 2.0 | **수정 시 해당 파일** 공개 의무 |
| `lofice/**` 신규 코드·문서·브랜드 | Lonex. Inc 독점 | 공개 의무 없음 (별도 계약) |
| UNO ID (`org.libreoffice.*`) | MPL 파생 내부 식별자 | 코드와 함께 (호환 목적 유지) |

상세: [`lofice/LICENSE.md`](../LICENSE.md)

---

## 2. MPL 2.0 핵심 의무 (배포 시)

### 2.1 MPL 헤더 유지

- MPL 2.0 파일 상단 주석 **삭제·변경 금지**
- `This Source Code Form is subject to the terms of the Mozilla Public License` 블록 유지
- Tier Headers rebrand는 `"lofice project"` → `"lofice project"` **문구만** 변경 (MPL 조항 자체는 유지)

### 2.2 수정 파일 소스 공개 (File-level copyleft)

MPL 파일을 **수정하여 배포**할 경우:

1. 수정된 MPL 파일의 **완전한 소스**를 수령자에게 제공
2. MPL 2.0 라이선스 전문 및 고지 포함
3. **전체 리포** 공개 의무는 없음 — **수정된 MPL 파일 단위** 의무

### 2.3 Larger Work (lofice + Lonex 독점 코드)

- MPL 모듈과 Lonex 독점 `lofice/**` 코드를 **하나의 바이너리**로 배포 가능
- MPL 파일만 소스 공개 의무; `lofice/**` 독점 부분은 별도 라이선스(Lonex. Inc) 적용

---

## 3. 소스 공개 범위 (권장)

배포·GitHub 공개 시 다음을 포함합니다:

| 포함 | 경로 예 |
|------|---------|
| ✅ MPL 수정본 | `framework/`, `sw/`, `sc/`, `sd/`, `sfx2/`, `desktop/`, `scp2/`, `sysui/` 등 rebrand 적용 모듈 |
| ✅ lofice 신규 | `lofice/**` |
| ✅ 빌드·설치 | `instsetoo_native/` (productlist: `lofice`) |
| ✅ 고지 | `NOTICE`, `lofice/LICENSE.md`, `lofice/COPYRIGHT-KR.md` |
| ⚠️ 선택 | `icon-themes/**/mainapp_*.svg`, `lofice_*.png` (브랜드 자산) |
| ❌ 제외 | `.env`, API 키, 서명 인증서, 내부 CI 시크릿 |

---

## 4. 브랜딩 vs MPL

| 항목 | 정책 |
|------|------|
| 사용자 표시명 | **lofice** / Lonex. Inc |
| 설치 파일명 | `lofice*_archive.zip` (`sysui/productlist.mk`) |
| MPL 헤더 "project" 문구 | lofice project (Headers tier) |
| UNO 서비스 ID | `org.libreoffice.*` **유지** (기능 호환) |
| TDF/lofice 상표 | 사용자 UI·설치 프로그램에서 **미사용** |

리브랜딩 스크립트: `lofice/scripts/rebrand-lofice.ps1`  
검증: `npm run verify-rebrand` ( `lofice/scripts` )

---

## 5. 서드파티 고지

배포 패키지 또는 저장소 루트에 포함:

- **lofice** — MPL 2.0 — Lonex. Inc
- **OpenOffice.org legacy** — Apache 2.0 (일부)
- 기타: 각 파일 헤더 및 `NOTICE` 참조

---

## 6. 체크리스트 (릴리스 전)

- [ ] MPL 헤더가 있는 수정 파일 목록 확인 (`git diff` / rebrand tier)
- [ ] `npm run verify-rebrand` 통과
- [ ] `npm run verify-ci` 통과
- [ ] Linux self-hosted 빌드 1회 (`lofice-build-selfhosted.yml`)
- [ ] 설치 UI 문자열 lofice 확인 (`instsetoo_native/inc_openoffice/.../*.ulf`)
- [ ] API 키·`.env` 미포함 확인
- [ ] `lofice/LICENSE.md`, `COPYRIGHT-KR.md` 최신

---

## 7. 문의

- **Rights / Licensing:** Lonex. Inc — shinkang888@gmail.com
- **Technical:** kangjunchul8@gmail.com

*MPL 2.0 전문: https://www.mozilla.org/MPL/2.0/*
