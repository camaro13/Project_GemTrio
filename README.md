# Project_GemCoop

<p align="center">
  <img src="https://img.shields.io/badge/Unreal%20Engine-5.7-4A90E2?style=for-the-badge&logo=unrealengine" alt="Unreal Engine 5.7" />
  <img src="https://img.shields.io/badge/C%2B%2B-Gameplay-00599C?style=for-the-badge&logo=c%2B%2B" alt="C++" />
  <img src="https://img.shields.io/badge/Genre-Co-op%20Action%20%2F%20Wave%20Survival-FF6B6B?style=for-the-badge" alt="Genre" />
</p>

Unreal Engine 5 기반의 협동형 액션/웨이브 서바이벌 게임 프로젝트입니다. 플레이어 역할, 보스 전투, 젬 기반 스킬 시스템, 로비와 메인 메뉴, 그리고 다양한 게임 모드 변형을 포함한 실험형 프로젝트입니다.

## 🚀 프로젝트 소개

이 프로젝트는 다음과 같은 핵심 요소를 중심으로 설계되었습니다.

- 3인칭 캐릭터 제어
- 협동 전투 시스템
- 웨이브 기반 몬스터 스폰 및 클리어 로직
- 젬 장착, 교환, 융합, 업그레이드 시스템
- 생존/부활/점수/등급 처리
- 로비, 메인 메뉴, HUD, 인벤토리 UI
- 전투, 플랫폼, 사이드 스크롤링 모드 변형

## ✨ 주요 기능

### 협동 전투 시스템
- 플레이어 캐릭터는 체력, 에너지, 스탯, 버프/디버프를 관리합니다.
- 기본 공격, 대시, 젬 기반 스킬, 궁극기 발동 구조를 포함합니다.
- 포메이션 역할과 팀 전투 흐름이 구현되어 있습니다.

### 웨이브 시스템
- `WaveManager`가 몬스터 스폰, 난이도 scaling, 보스 등장, 레벨 종료 로직을 제어합니다.
- 각 웨이브마다 체력, 공격력, 수량, 보상 구조가 확장됩니다.
- 클리어/실패 조건과 다음 웨이브 전환 흐름을 처리합니다.

### 젬/융합/업그레이드 시스템
- 젬 수집, 교환, 융합과 같은 자원 관리 루프를 구성합니다.
- 점수, 생존 시간, 보스 처치 결과를 기반으로 최종 등급을 산출합니다.
- 보상 시스템은 골드, 업그레이드 포인트, 진행 지표를 반영합니다.

### UI 및 로비 구성
- 메인 메뉴, 로비, HUD, 인벤토리 행 위젯을 포함합니다.
- 생존 시간, 몬스터 HP, 점수, 보스 정보 등의 화면을 제공합니다.

### 모드 변형 구조
- `Variant_Combat`
- `Variant_Platforming`
- `Variant_SideScrolling`

공통 시스템을 유지하면서 각기 다른 플레이 스타일을 확장할 수 있도록 설계되어 있습니다.

## 🧩 기술 스택

- Unreal Engine 5.7
- C++ Gameplay Framework
- UMG UI
- Enhanced Input
- StateTree / GameplayStateTree
- NavigationSystem / AIModule
- DataTable 기반 데이터 설계

## 📁 프로젝트 구조

```text
Project_GemCoop/
├── Config/                     # 프로젝트 설정
├── Content/                    # 맵, 에셋, UI, 블루프린트
├── Source/
│   └── Project_GemCoop/
│       ├── Variant_Combat/
│       ├── Variant_Platforming/
│       ├── Variant_SideScrolling/
│       ├── *.h / *.cpp         # 핵심 게임 로직
│       ├── Project_GemCoop.Build.cs
│       └── ...
├── Binaries/
├── Intermediate/
├── Plugins/
├── Saved/
├── Project_GemCoop.uproject
├── Project_GemCoop.sln
├── README.md
└── .gitignore
```

## 📌 핵심 파일

- `Project_GemCoop.uproject` : 프로젝트 진입 파일
- `Source/Project_GemCoop/Project_GemCoopCharacter.h` : 캐릭터 기본 구조
- `Source/Project_GemCoop/Project_GemCoopGameMode.h` : 게임 상태 및 결과 처리
- `Source/Project_GemCoop/Project_GemCoopWaveManager.h` : 웨이브 및 스폰 관리
- `Source/Project_GemCoop/Project_GemCoopGemDataSubsystem.*` : 젬 데이터 관리
- `Source/Project_GemCoop/Project_GemCoopMonsterCharacter.*` : 몬스터 로직

## 🛠️ 요구사항

- Unreal Engine 5.7 이상
- Visual Studio 2022
- C++ 개발 워크로드 설치
- Windows 환경 권장

## ▶️ 실행 방법

1. Unreal Engine 5.7 런처를 실행합니다.
2. `Project_GemCoop.uproject` 파일을 엽니다.
3. 프로젝트를 로드한 뒤 에디터에서 맵을 선택합니다.
4. 필요한 경우 프로젝트 파일을 다시 생성하거나 빌드합니다.
5. 에디터에서 플레이를 실행합니다.

## 🧠 개발 포인트

- 공통 시스템은 C++ 모듈로 구현되어 있으며, 일부 UI와 로직은 블루프린트와 에셋으로 보완됩니다.
- `Content` 폴더에는 맵, UI, 몬스터, 스폰 포인트, 데이터 테이블 등 주요 자산이 포함됩니다.
- `Variant_*` 디렉터리를 통해 같은 게임의 여러 플레이 스타일을 확장할 수 있습니다.

## 🏗️ 프로젝트 성격

이 프로젝트는 단순 데모가 아니라 Unreal Engine 기반의 게임 시스템 설계와 실험 프로젝트에 가깝습니다. 특히 다음 항목을 연습하고 확장하는 데 적합합니다.

- 웨이브 시스템 설계
- 협동형 전투 로직
- 상태 관리 및 데이터 테이블 연동
- UI/ HUD 구조 설계
- 게임 루프와 점수 계산
- C++와 블루프린트 결합 구조

## 📄 라이선스

본 프로젝트는 개인 개발/실험 목적의 프로젝트이며, 별도 라이선스가 명시되지 않은 경우 프로젝트 소유자 기준으로 관리됩니다.

## 👤 프로젝트 정보

- 프로젝트명: `Project_GemCoop`
- 엔진: Unreal Engine 5
- 언어: C++
- 장르: 협동 액션 / 웨이브 서바이벌

## 📎 추가로 가능

- GitHub용 배너 이미지 추가
- 한국어/영어 README 버전 분리
- 설치 가이드 상세화
- 스크린샷 및 게임 플레이 섹션 추가
- 라이선스/기여 문서 포함
