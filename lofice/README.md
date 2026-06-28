# lofice — AI-Friendly Modern Office Suite

## Technology Policy

| 항목 | 정책 |
|------|------|
| 언어 | **모던 C++17/20 유지** — 순수 C 전환 금지 |
| 아키텍처 | UNO 컴포넌트 모델 + VCL/weld UI 유지 |
| Rust | 선택적 래핑(Wrapping) — 코어 대체 아님 |
| AI Hooking | `lofice::IEventListener` 인터페이스로 이벤트 후킹 |

## Branding

| 항목 | 값 |
|------|-----|
| 제품명 | lofice |
| 권리자 | Lonex. Inc — shinkang888@gmail.com |
| 발명자 | kangjunchul8@gmail.com |
| 저작권 | 대한민국 저작권 등록 |
| 로고 | `lofice/assets/lofice-logo-master.png` |

## Rebranding Scripts

```powershell
.\lofice\scripts\rebrand-lofice.ps1 -Tier Display    # UI 표시명
.\lofice\scripts\rebrand-lofice.ps1 -Tier Headers     # 파일 헤더
.\lofice\scripts\generate-icons.ps1
.\lofice\scripts\deploy-icons.ps1
```

## IP Protection

- [LICENSE.md](LICENSE.md)
- [COPYRIGHT-KR.md](COPYRIGHT-KR.md)

## Important: UNO Compatibility

`org.libreoffice.*` UNO 서비스 식별자는 **기능 호환**을 위해 코드 내부에 잔존할 수 있습니다.  
사용자 대면 UI·설치 프로그램·문서에는 LibreOffice 표기를 사용하지 않습니다.
