# Phase 1: LibreOffice 아키텍처 매핑 — VCL ↔ UNO 연결 구조

> **목적:** lofice 재설계 전, AI가 전체 구조를 먼저 이해하도록 UI(VCL)와 데이터(UNO/내부 모델)의 연결 관계를 정리한다.  
> **범위:** 코드 수정 없음. 파일 간 종속성 트리 + 데이터 흐름만 기록.  
> **기준 리포:** `libreoffice/` (LibreOffice upstream)

---

## 1. 한눈에 보는 전체 계층

LibreOffice는 **이중 프레임워크** 위에 애플리케이션 모듈이 올라간 구조다.

```
┌─────────────────────────────────────────────────────────────────┐
│  Application Modules                                            │
│  sw (Writer)  │  sc (Calc)  │  sd (Draw/Impress)               │
│  SwDoc        │  ScDocument │  SdDrawDocument                   │
└───────────────┬─────────────────┬───────────────────────────────┘
                │                 │
┌───────────────▼─────────────────▼───────────────────────────────┐
│  SFX2 Legacy Framework (sfx2/)                                  │
│  SfxObjectShell · SfxViewShell · SfxDispatcher · SfxBindings    │
│  SfxBaseModel (UNO XModel facade)                               │
└───────────────┬─────────────────────────────────────────────────┘
                │
┌───────────────▼─────────────────────────────────────────────────┐
│  framework/ (UNO UI Chrome)                                     │
│  Toolbars · Menus · Accelerators → .uno: commands               │
└───────────────┬─────────────────────────────────────────────────┘
                │
┌───────────────▼─────────────────────────────────────────────────┐
│  Shared UI (svx/, svt/, svtools/)                               │
│  Rulers · Toolbars helpers · Drawing helpers                     │
└───────────────┬─────────────────────────────────────────────────┘
                │
┌───────────────▼─────────────────────────────────────────────────┐
│  weld:: (Modern UI abstraction)  +  VclBuilder (Legacy .ui)     │
└───────────────┬─────────────────────────────────────────────────┘
                │
┌───────────────▼─────────────────────────────────────────────────┐
│  VCL — Visual Class Library (vcl/)                              │
│  vcl::Window · Control · OutputDevice · Application             │
└───────────────┬─────────────────────────────────────────────────┘
                │ SalInstance vtable
┌───────────────▼─────────────────────────────────────────────────┐
│  Platform Plugins (vclplug_gtk3, vclplug_win, vclplug_qt6, svp) │
└─────────────────────────────────────────────────────────────────┘

        ═══════════════ UNO API Layer (병렬 축) ═══════════════

┌─────────────────────────────────────────────────────────────────┐
│  Language Bindings: pyuno, javaunohelper, jsuno …              │
└───────────────┬─────────────────────────────────────────────────┘
                │
┌───────────────▼─────────────────────────────────────────────────┐
│  App UNO Wrappers                                               │
│  SwXTextDocument · ScModelObj · SdXImpressDocument              │
└───────────────┬─────────────────────────────────────────────────┘
                │ inherits
┌───────────────▼─────────────────────────────────────────────────┐
│  SfxBaseModel (sfx2/) — 공통 XModel3, XStorable, XModifiable …   │
└───────────────┬─────────────────────────────────────────────────┘
                │ m_pObjectShell
┌───────────────▼─────────────────────────────────────────────────┐
│  SfxObjectShell → SwDocShell / ScDocShell / DrawDocShell          │
└─────────────────────────────────────────────────────────────────┘
```

**핵심 인사이트:** UI와 모델은 **직접 1:1로 묶이지 않는다**. SFX2의 View/Shell/Dispatcher 계층이 중간 브릿지이며, UNO는 주로 **외부 API·매크로·프레임워크 UI**용 facade다. 편집 창 입력은 슬롯 없이 C++ 모델을 직접 수정하는 경로도 존재한다.

---

## 2. 모듈 종속성 트리 (빌드 순서)

`Library_*.mk`의 `gb_Library_use_libraries` 기준 요약.

```
sal / salhelper
 └── cppu / cppuhelper          ← UNO 런타임 (Any, typelib, WeakImplHelper)
      └── registry / store / stoc
           └── udkapi             ← types.rdb (저수준 UNO IDL)
                └── offapi        ← offapi.rdb (text.*, sheet.*, drawing.* IDL)
                     └── tools, svl, sot, tl, i18nutil, comphelper
                          └── vcl                    ← UI + GDI + weld 구현
                               ├── toolkit (tk)     ← VCL → UNO AWT 래퍼
                               ├── svt / svtools
                               └── framework (fwk)   ← UNO 툴바/메뉴 (vcl 의존, sfx2 없음)
                                    └── sfx2 (sfx)   ← 문서/뷰/슬롯 프레임워크
                                         ├── svx / svxcore / editeng / drawinglayer
                                         ├── sw     ← Writer
                                         ├── sc     ← Calc
                                         └── sd     ← Draw / Impress
```

| 모듈 | Makefile | 주요 산출물 | 직접 의존 (요약) |
|------|----------|------------|-----------------|
| `vcl` | `vcl/Module_vcl.mk`, `vcl/Library_vcl.mk` | `Library_vcl`, `Library_vclplug_*` | sal, basegfx, comphelper, svl |
| `toolkit` | `toolkit/Module_toolkit.mk` | `Library_tk` | vcl, svl, comphelper |
| `framework` | `framework/Module_framework.mk` | `Library_fwk` | vcl, cppuhelper, comphelper |
| `sfx2` | `sfx2/Module_sfx2.mk` | `Library_sfx` | vcl, fwk, svl, tk, cppuhelper |
| `sw` | `sw/Library_sw.mk` | `Library_sw` | sfx, vcl, fwk, svx, editeng, docmodel |
| `sc` | `sc/Library_sc.mk` | `Library_sc` | sfx, vcl, fwk, svx, editeng, for |
| `sd` | `sd/Library_sd.mk` | `Library_sd` | sfx, vcl, fwk, svx |

---

## 3. VCL UI 스택 — 파일·클래스 종속성

### 3.1 VCL 내부 계층

| 계층 | 핵심 클래스 | 헤더/소스 | 역할 |
|------|------------|----------|------|
| OS 추상화 | `SalInstance`, `SalFrame`, `SalGraphics` | `vcl/inc/salinst.hxx`, `vcl/inc/salframe.hxx` | 플랫폼 플러그인 vtable |
| 플러그인 로딩 | `CreateSalInstance()` | `vcl/source/app/salplug.cxx` | gtk3/qt6/win/svp 등 선택 |
| 앱 루프 | `Application`, `InitVCL()` | `include/vcl/svapp.hxx`, `vcl/source/app/svmain.cxx` | Yield, SolarMutex, 스케줄러 |
| 위젯 트리 | `vcl::Window` | `include/vcl/window.hxx` | 모든 위젯 루트, 이벤트 가상함수 |
| 컨트롤 | `Control` | `include/vcl/ctrl.hxx` | 포커스, 가속키, 레이아웃 |
| 문서 편집 영역 | `vcl::DocWindow` | `include/vcl/DocWindow.hxx` | Writer/Calc/Draw 편집 창 베이스 |
| 렌더링 | `OutputDevice` | `include/vcl/outdev.hxx` | Window, VirtualDevice, Printer |
| 수명 관리 | `VclPtr<T>` | `include/vcl/vclptr.hxx` | ref-count + dispose() |

### 3.2 weld 추상화 (신규 UI 권장 경로)

| 클래스 | 헤더 | VCL 구현 | 역할 |
|--------|------|---------|------|
| `weld::Widget` | `include/vcl/weld/Widget.hxx` | `vcl/source/weld/*` | signal/link 기반 이벤트 |
| `weld::Window` | `include/vcl/weld/Window.hxx` | `SalInstanceWindow` (`vcl/inc/salvtables.hxx`) | VCL Window 어댑터 |
| `weld::Builder` | `include/vcl/weld/Builder.hxx` | `Application::CreateBuilder()` (`vcl/source/window/builder.cxx`) | `.ui` XML → 위젯 트리 |
| `weld::DialogController` | `include/vcl/weld/DialogController.hxx` | — | 다이얼로그 MVC |

**종속 방향:** `weld::` → (어댑터) → `vcl::Window` → `SalFrame` → OS

### 3.3 toolkit (UNO AWT — 스크립트용 UI)

| 항목 | 경로 | 설명 |
|------|------|------|
| 모듈 README | `toolkit/README.md` | "VCL의 UNO 래퍼" |
| 구현 | `toolkit/source/awt/vclxwindow*.cxx` | `VCLXWindow`, `VCLXTopWindow` |
| 의존 | `toolkit/Library_tk.mk` | vcl, svl, comphelper |

**주의:** lofice 코어 UI 재설계 시 toolkit은 **매크로/확장 UNO AWT** 경로이며, 메인 앱 UI(weld/VCL)와는 별도 축이다.

---

## 4. UNO 모델 스택 — 파일·클래스 종속성

### 4.1 UNO 인프라

| 계층 | 모듈 | 핵심 파일 | 역할 |
|------|------|----------|------|
| IDL (저수준) | `udkapi` | `udkapi/Module_udkapi.mk` | XInterface, Any, XComponentContext |
| IDL (앱) | `offapi` | `offapi/UnoApi_offapi.mk` | text.*, sheet.*, drawing.* |
| IDL → C++ | `codemaker` | `codemaker/` | cppumaker → `workdir/UnoApi/offapi/.../*.hpp` |
| 런타임 | `cppu` | `cppu/Module_cppu.mk` | Any, Sequence, typelib |
| 헬퍼 | `cppuhelper` | `cppuhelper/Module_cppuhelper.mk` | WeakImplHelper, bootstrap |
| 리플렉션 | `stoc` | `stoc/Module_stoc.mk` | 서비스 매니저, introspection |
| Python | `pyuno` | `pyuno/README.md` | Python UNO 바인딩 |

### 4.2 SfxBaseModel — 모든 문서 UNO Model의 공통 조상

```
SfxObjectShell (C++ 문서 셸)
    │  SetBaseModel() at init
    ▼
SfxBaseModel                    include/sfx2/sfxbasemodel.hxx
                                sfx2/source/doc/sfxbasemodel.cxx
    │  XModel3, XStorable2, XModifiable2, XPrintable, XCloseable …
    ▼
[앱별 파생]
    ├── SwXTextDocument         sw/inc/unotxdoc.hxx
    ├── ScModelObj              sc/inc/docuno.hxx
    └── SdXImpressDocument      sd/inc/unomodel.hxx
```

**SetBaseModel 호출 지점 (DocShell 초기화):**

| 앱 | 파일 | 코드 |
|----|------|------|
| Writer | `sw/source/uibase/app/docshini.cxx:385` | `SetBaseModel(new SwXTextDocument(this))` |
| Calc | `sc/source/ui/docshell/docsh.cxx:2936` | `ScModelObj::CreateAndSet(this)` |
| Draw | `sd/source/ui/docshell/docshell.cxx:104` | `SetBaseModel(new SdXImpressDocument(this, bClipboard))` |

### 4.3 앱별 내부 모델 ↔ UNO 래퍼 매핑

#### Writer (sw)

```
SwDocShell (sw/inc/docsh.hxx)
    └── SwDoc (내부 텍스트 모델)
            ▲
SwXTextDocument (sw/source/uibase/uno/unotxdoc.cxx)
    └── getText() → SwXBodyText : SwXText (sw/inc/unotextbodyhf.hxx)
            └── insertString() → SwPaM → SwNodes/SwTextNode
```

| UNO 인터페이스 | IDL | C++ 구현 | 소스 |
|---------------|-----|---------|------|
| `XTextDocument` | `offapi/.../text/XTextDocument.idl` | `SwXTextDocument` | `sw/source/uibase/uno/unotxdoc.cxx` |
| `XText` | `offapi/.../text/XText.idl` | `SwXBodyText` / `SwXText` | `sw/source/core/unocore/unotext.cxx` |
| 서비스 등록 | — | `com.sun.star.comp.Writer.TextDocument` | `sw/util/sw.component` |

#### Calc (sc)

```
ScDocShell (sc/source/ui/inc/docsh.hxx)
    └── ScDocument
            ▲
ScModelObj (sc/source/ui/unoobj/docuno.cxx)
    └── getSheets() → ScTableSheetsObj
            └── getByName() → ScTableSheetObj (sc/inc/cellsuno.hxx)
                    └── getCellByPosition() → ScCellObj
```

| UNO 인터페이스 | C++ 구현 | 소스 |
|---------------|---------|------|
| `XSpreadsheetDocument` | `ScModelObj` | `sc/source/ui/unoobj/docuno.cxx` |
| `XSpreadsheets` | `ScTableSheetsObj` | `sc/source/ui/unoobj/docuno.cxx` |
| `XSpreadsheet` | `ScTableSheetObj` | `sc/inc/cellsuno.hxx` |
| `XCell` | `ScCellObj` | `sc/inc/cellsuno.hxx` |
| 서비스 등록 | `com.sun.star.comp.Calc.SpreadsheetDocument` | `sc/util/sc.component` |

#### Draw / Impress (sd)

```
DrawDocShell (sd/source/ui/inc/DrawDocShell.hxx)
    └── SdDrawDocument (SdrModel 파생)
            ▲
SdXImpressDocument (sd/source/ui/unoidl/unomodel.cxx)
    └── getDrawPages() → SdDrawPage (sd/source/ui/unoidl/unopage.cxx)
            └── getShapes() → SdXShape (sd/source/ui/unoidl/unoobj.cxx)
```

| UNO 인터페이스 | C++ 구현 | 소스 |
|---------------|---------|------|
| `XDrawPagesSupplier` | `SdXImpressDocument` | `sd/inc/unomodel.hxx` |
| `XDrawPage` | `SdDrawPage` | `sd/source/ui/unoidl/unopage.cxx` |
| `XShape` | `SdXShape` | `sd/source/ui/unoidl/unoobj.cxx` |
| 서비스 등록 | `DrawingDocument`, `PresentationDocument` | `sd/util/sd.component` |

---

## 5. UI ↔ 모델 브릿지 — SFX2 MVC

LibreOffice의 "전통 MVC"는 SFX2에 구현되어 있다.

```
SfxObjectShell ───────────── Document / C++ Model (SwDoc 등)
    │
    ├── SfxBaseModel ───────── UNO XModel facade
    │
    └── SfxViewFrame (프레임 1개 = 창 1개)
            ├── SfxViewShell ─── View (SwView, ScTabViewShell, sd::ViewShellBase)
            │       ├── vcl::Window / weld::Window (편집 영역)
            │       └── SubShells (SwWrtShell, ScEditShell, sd::View …)
            ├── SfxDispatcher ── Shell 스택 + Slot 실행
            ├── SfxBindings ─── UI 상태(활성/비활성) 동기화
            └── SfxBaseController ─ UNO XController + XDispatchProvider
```

### 5.1 SFX2 핵심 클래스 파일 맵

| 클래스 | 헤더 | 구현 (주요) | 역할 |
|--------|------|------------|------|
| `SfxShell` | `include/sfx2/shell.hxx` | — | 슬롯 Exec/State 등록 (`SFX_IMPL_INTERFACE`) |
| `SfxViewShell` | `include/sfx2/viewsh.hxx` | `sfx2/source/view/` | 뷰 1개; `GetWindow()`, `KeyInput()` |
| `SfxViewFrame` | `include/sfx2/viewfrm.hxx` | `sfx2/source/view/viewfrm.cxx` | Dispatcher, Bindings 소유 |
| `SfxBindings` | `include/sfx2/bindings.hxx` | `sfx2/source/control/bindings.cxx` | SlotID ↔ ControllerItem |
| `SfxDispatcher` | `include/sfx2/dispatch.hxx` | `sfx2/source/control/dispatch.cxx` | Shell 스택 Push/Pop, 슬롯 실행 |
| `SfxRequest` | `include/sfx2/request.hxx` | — | 슬롯 파라미터 (`SfxItemSet`) |
| `SfxObjectShell` | `include/sfx2/objsh.hxx` | `sfx2/source/doc/` | 문서 셸, `GetModel()` |
| `SfxBaseController` | `include/sfx2/sfxbasecontroller.hxx` | `sfx2/source/control/sfxbasecontroller.cxx` | UNO Controller, ViewShell* 보유 |
| `SfxOfficeDispatch` | — | `sfx2/source/control/unoctitm.cxx` | `.uno:` URL → SlotID |
| SDI 슬롯 | — | `sfx2/sdi/*.sdi` + `svidl` | ExecMethod/StateMethod → C++ 함수 포인터 |

### 5.2 framework — UNO UI Chrome

| 파일 | 역할 |
|------|------|
| `framework/source/uielement/toolbarmanager.cxx` | 툴바 → `XDispatchProvider::queryDispatch` |
| `framework/source/uielement/toolbarsmenucontroller.cxx` | 메뉴 → UNO dispatch |
| `framework/source/accelerators/*` | 가속키 → `.uno:` 명령 |

**종속:** framework는 **sfx2 없이** vcl만 사용. 실제 슬롯 실행은 sfx2의 `SfxOfficeDispatch`까지 내려간다.

### 5.3 앱별 View 클래스 매핑

| 앱 | SfxViewShell | 편집 vcl::Window | Document Shell | SubShell (모델 조작) |
|----|-------------|-----------------|----------------|---------------------|
| Writer | `SwView` (`sw/inc/view.hxx`) | `SwEditWin` (`sw/source/uibase/inc/edtwin.hxx`) | `SwDocShell` | `SwWrtShell`, `SwViewShell` (레이아웃) |
| Calc | `ScTabViewShell` (`sc/source/ui/inc/tabvwsh.hxx`) | `sc::GridWindow` | `ScDocShell` | `ScEditShell`, `ScDrawShell` |
| Impress | `sd::ViewShellBase` (`sd/source/ui/inc/ViewShellBase.hxx`) | `sd::Window` | `sd::DrawDocShell` | stacked `sd::ViewShell` → `sd::View` (SdrView) |

**Writer 이중 View 주의:**

- `SwView` = SFX ViewShell (UI 프레임, 메뉴/툴바 연동)
- `SwViewShell` = 레이아웃/페인트 엔진 (별도 계층, `sw/inc/viewsh.hxx`)

---

## 6. 데이터 흐름 — 4가지 경로

### 경로 A: 메뉴 / 툴바 / 가속키 (가장 일반적)

```
[사용자] 메뉴·툴바·가속키 클릭
    │
    ▼
framework: ToolbarsMenuController / ToolbarManager
    │  queryDispatch(".uno:Command")
    ▼
SfxBaseController (XDispatchProvider)
    │
    ▼
SfxOfficeDispatch::dispatch()          sfx2/source/control/unoctitm.cxx
    │  .uno: URL → SlotID (SID_*)
    ▼
SfxBindings::Execute()
    │
    ▼
SfxDispatcher::Execute() → Call_Impl()
    │  SfxSlot::GetExecFnc()
    ▼
SfxShell::Exec*(SfxRequest&)           예: SwView::ExecPaste
    │
    ▼
SubShell (SwWrtShell / ScDocFunc / sd::View)
    │
    ▼
C++ Model (SwDoc / ScDocument / SdDrawDocument)
    │
    ▼
SfxBindings::Invalidate()              UI 상태(Undo/Paste 가능 등) 갱신
```

**예시 (Writer Paste):** `sw/sdi/*.sdi` → `SID_PASTE` → `SwView::ExecPaste` → `SwWrtShell` → `SwDoc`

---

### 경로 B: 편집 창 직접 입력 (키보드/마우스)

```
[OS] Win32 / GTK / Qt / X11 이벤트
    │
    ▼
SalFrame::CallCallback()               vcl/win/window/salframe.cxx (플랫폼별)
    │
    ▼
ImplWindowFrameProc()                  vcl/source/window/winproc.cxx
    │
    ├─► ImplCallPreNotify() → Window::PreNotify() (부모→자식)
    │       └─► SfxFrameWindow_Impl::PreNotify()    sfx2/source/view/frame2.cxx
    │               └─► SfxViewShell::HandleNotifyEvent_Impl()
    │                       └─► SfxBaseController::HandleEvent_Impl()
    │
    └─► pChild->KeyInput() / MouseButtonDown()
            │
            ▼
        SwEditWin::KeyInput() / ScTabViewShell / sd::ViewShell
            │
            ▼
        SwWrtShell / ScEditShell / sd::View (SdrView)
            │
            ▼
        SwDoc / ScDocument / SdDrawDocument  (슬롯 디스패치 **우회**)
            │
            ▼
        Invalidate / Repaint → SalEvent::Paint → OutputDevice
```

**핵심:** 타이핑·드래그는 SFX 슬롯을 거치지 않고 C++ 모델을 직접 변경한다.

---

### 경로 C: UNO API / 매크로 / 외부 스크립트

```
[외부] Python/Java/Basic UNO 클라이언트
    │
    ▼
Service Manager (stoc) ← .component 등록 (sw.component, sc.component, sd.component)
    │
    ▼
Factory: Writer_SwTextDocument_get_implementation  sw/source/uibase/uno/unodoc.cxx
    │
    ▼
SfxObjectShell 생성 → GetModel()
    │
    ▼
SwXTextDocument / ScModelObj / SdXImpressDocument
    │
    ▼
SwXText::insertString() / ScTableSheetObj / SdDrawPage …
    │
    ▼
SwDoc / ScDocument / SdDrawDocument
    │
    ▼ (역방향)
SwClient::Notify / SfxBroadcaster → UNO 래퍼 dispose·갱신
```

**SolarMutex:** UNO 호출은 `SolarMutexGuard`로 메인 스레드 직렬화.

---

### 경로 D: weld 다이얼로그

```
[사용자] 다이얼로그 OK / Apply
    │
    ▼
weld::DialogController (핸들러)
    │
    ├─► SfxDispatcher::Execute(SID_*)     → 경로 A 합류
    └─► 모델 직접 호출                     → 경로 B/C와 유사
```

**빌드:** `Application::CreateBuilder()` → `SalInstance::CreateBuilder()` → `.ui` XML 파싱

---

## 7. 파일 간 종속성 트리 (핵심 경로만)

### 7.1 입력 이벤트 체인

```
vclplug_* (플랫폼)
  └── SalFrame
        └── winproc.cxx (ImplWindowFrameProc)
              ├── frame2.cxx (SfxFrameWindow_Impl::PreNotify)
              │     └── viewsh.hxx (SfxViewShell)
              │           └── [앱] view.hxx (SwView) / tabvwsh.hxx (ScTabViewShell)
              │                 └── edtwin.hxx (SwEditWin) / GridWindow
              │                       └── [모델] SwWrtShell → SwDoc
              └── ctrl.hxx (Control — 다이얼로그 위젯)
```

### 7.2 명령(슬롯) 실행 체인

```
/uiconfig/*.xml (UI 정의)
  └── framework (toolbarmanager.cxx)
        └── sfxbasecontroller.hxx (SfxBaseController)
              └── unoctitm.cxx (SfxOfficeDispatch)
                    └── dispatch.cxx (SfxDispatcher)
                          └── shell.hxx (SfxShell — SwView, SwWrtShell, …)
                                └── [모델] SwDoc / ScDocument / SdDrawDocument
```

### 7.3 UNO 문서 접근 체인

```
offapi/*.idl
  └── codemaker (cppumaker) → *.hpp
        └── unotxdoc.hxx / docuno.hxx / unomodel.hxx
              └── sfxbasemodel.hxx (SfxBaseModel)
                    └── objsh.hxx (SfxObjectShell)
                          └── docsh.hxx (SwDocShell / ScDocShell / DrawDocShell)
                                └── SwDoc / ScDocument / SdDrawDocument
```

### 7.4 앱별 "대표 파일" 종속 트리

#### Writer

```
sw/inc/view.hxx (SwView : SfxViewShell)
  ├── sw/source/uibase/inc/edtwin.hxx (SwEditWin : vcl::DocWindow)
  │     └── include/vcl/DocWindow.hxx → include/vcl/window.hxx
  ├── sw/inc/docsh.hxx (SwDocShell : SfxObjectShell)
  │     ├── sw/source/uibase/app/docshini.cxx → SetBaseModel(SwXTextDocument)
  │     └── SwDoc (내부 모델)
  └── sw/inc/unotxdoc.hxx (SwXTextDocument : SfxBaseModel)
        └── include/sfx2/sfxbasemodel.hxx
```

#### Calc

```
sc/source/ui/inc/tabvwsh.hxx (ScTabViewShell : SfxViewShell)
  ├── sc/source/ui/inc/docsh.hxx (ScDocShell)
  │     ├── sc/source/ui/docshell/docsh.cxx → ScModelObj::CreateAndSet
  │     └── ScDocument
  └── sc/inc/docuno.hxx (ScModelObj)
        └── sc/inc/cellsuno.hxx (ScTableSheetObj, ScCellObj)
```

#### Impress

```
sd/source/ui/inc/ViewShellBase.hxx (: SfxViewShell)
  ├── sd/source/ui/inc/DrawDocShell.hxx
  │     ├── sd/source/ui/docshell/docshell.cxx → SetBaseModel(SdXImpressDocument)
  │     └── SdDrawDocument
  └── sd/inc/unomodel.hxx (SdXImpressDocument)
        └── sd/source/ui/unoidl/unopage.cxx (SdDrawPage)
```

---

## 8. 컴포넌트 등록 (.component) — UNO 진입점

| 파일 | Implementation Name | Constructor |
|------|---------------------|-------------|
| `sw/util/sw.component` | `com.sun.star.comp.Writer.TextDocument` | `sw/source/uibase/uno/unodoc.cxx` |
| `sc/util/sc.component` | `com.sun.star.comp.Calc.SpreadsheetDocument` | `sc/source/ui/unoobj/unodoc.cxx` |
| `sd/util/sd.component` | Drawing / Presentation Document | `sd/source/ui/unoidl/unodoc.cxx` |
| 공통 팩토리 | — | `sfx2/source/doc/sfxmodelfactory.cxx` |

---

## 9. lofice Phase 2+ 시사점 (번역 전 체크리스트)

| # | 관찰 | lofice 대응 방향 |
|---|------|-----------------|
| 1 | UI→모델 주 통로는 **SFX2 슬롯 디스patch** | 슬롯/Exec 계층을 API-first 레이어로 분리 검토 |
| 2 | UNO는 **facade** — 내부는 C++ 모델 | UNO 래퍼(`SwX*`)와 코어(`SwDoc`) 경계 명확히 |
| 3 | **weld** ≠ VCL — 어댑터 패턴 | 새 UI는 weld 또는 완전 교체 추상화 선택 |
| 4 | Writer **이중 View** (`SwView` vs `SwViewShell`) | 레이아웃 엔진과 UI 프레임 분리 유지 |
| 5 | Impress **3층 View** (ViewShellBase → ViewShell → View) | sd 모듈 리팩토링 시 최대 복잡도 구간 |
| 6 | 편집 입력은 **슬롯 우회** | "모든 변경이 UNO를 탄다"는 가정 금지 |
| 7 | `SfxBaseModel` = 모든 앱 공통 | lofice 공통 Document API의 natural anchor |
| 8 | MPL 라이선스 헤더 | 파일 최상단 유지 (프로젝트 규칙) |

---

## 10. 참고 README / 문서

| 경로 | 내용 |
|------|------|
| `README.md` (루트) | 모듈 개요 (sal, vcl, framework, sfx2, sw, sc, sd) |
| `vcl/README.md` | VCL, SalInstance, GDIMetaFile |
| `vcl/README.lifecycle.md` | VclPtr / dispose 수명 |
| `vcl/README.scheduler.md` | 이벤트 큐 |
| `toolkit/README.md` | UNO AWT 래퍼 |
| `sfx2/README.md` | SlotID, SDI, SfxMedium |
| `framework/README.md` | UNO UI chrome |
| `sw/README.md` | SwDoc, SwClient, 필드 |
| `sc/README.md` | ScDocument |
| `sd/README.md` | SdDrawDocument, Impress=Draw 확장 |
| `sfx2/doc/sfx2doc.html` | SFX2 상세 (HTML) |
| https://docs.libreoffice.org/ | 모듈별 README 웹 미러 |

---

## 11. 다음 Phase 제안

1. **Phase 2:** Writer(`sw`) 단일 모듈 심층 — `SwEditWin` → `SwWrtShell` → `SwDoc` 한 기능(예: Bold) end-to-end 추적
2. **Phase 3:** SFX2 슬롯 → lofice API 매핑 테이블 (SID ↔ 공개 API)
3. **Phase 4:** weld/VCL 교체 가능 경계면 정의 (headless/LibreOfficeKit 참고: `static/README.wasm.md`)

---

*문서 생성: Phase 1 Architecture Mapping — 코드 변경 없음.*
