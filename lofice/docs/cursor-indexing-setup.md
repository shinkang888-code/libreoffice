# Cursor 코드베이스 색인 — lofice 워크스페이스

LibreOffice 전체 리포는 파일 수가 많아 Cursor `@Codebase` 품질·속도를 위해 **제외 규칙**을 두었습니다.

## 설정 파일 (리포 루트)

| 파일 | 역할 |
|------|------|
| `.cursorignore` | AI·grep **완전 차단** (비밀, 빌드 산출물) |
| `.cursorindexingignore` | **색인만 제외** (`external/`, QA 대용량 등) — `@file`로는 읽기 가능 |

## UI에서 보이는 버튼

| 상태 | 표시 | 동작 |
|------|------|------|
| 미색인 / 캐시 삭제 후 | **Start indexing** | 처음부터 색인 |
| **100% 완료** | **Sync** / **Delete Index** | Start indexing **없음** (정상) |

**100%인데 "Codebase not indexed" 배너가 보이면** → **Sync** 한 번 클릭 (ignore 규칙 반영).

## ignore 반영 절차 (1회)

1. `Ctrl+Shift+P` → **Developer: Reload Window** (선택)
2. **Cursor Settings → Indexing & Docs**
3. **Sync** 클릭 → 100%까지 대기
4. (선택) **View included files** 로 파일 수 확인 — `external/` 등이 빠졌는지 확인

`.cursorindexingignore` 적용 후 파일 수는 **54k → 약 4만** 수준으로 줄어드는 것이 정상입니다.

## 수동 재색인

명령 팔레트: `Ctrl+Shift+P` → **Cursor: Resync Index** (또는 Reindex)

## lofice 전용 Neon 색인 (별도)

Cursor 색인과 별개로 `lofice/`만 Postgres에 색인합니다.

```powershell
cd lofice\scripts
npm install
npm run index          # .env.local DATABASE_URL 자동 로드
npm run search -- "AiAssistantPanel"
```

의미 검색 (OpenAI 키 필요):

```powershell
# .env.local 에 OPENAI_API_KEY 추가 후
npm run embed
npm run search -- --semantic "conversation export import"
```

## 제외 대상 요약

- 빌드: `workdir/`, `instdir/`, `in/`, `out/`
- 비밀: `.env.local`, `*.key`, `credentials.json`
- 저신호 bulk: `external/`, `odk/`, `qadevOOo/`, `android/`, `ios/`

자세한 Neon 설정: [neon-codebase-index.md](./neon-codebase-index.md)
