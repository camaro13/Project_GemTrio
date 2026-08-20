# Project_GemCoop

Unreal Engine 5 기반의 협동형 액션/웨이브 서바이벌 프로젝트입니다. 여러 플레이어 역할, 보스 전투, 젬 기반 스킬 시스템, 로비와 메인 메뉴, 다양한 게임 모드 변형을 포함한 실험형 멀티플레이/싱글플레이 프로젝트로 구성되어 있습니다.

## 프로젝트 소개

이 프로젝트는 다음과 같은 핵심 요소를 중심으로 설계되었습니다.

- 3인칭 캐릭터 컨트롤
- 협동 전투 시스템
- 웨이브 기반 몬스터 스폰 및 클리어 로직
- 젬 장착/융합/교환/업그레이드 개념
- 플레이어 생존, 부활, 점수/등급 계산
- 로비, 메인 메뉴, HUD, 인벤토리 UI
- 서브 모드: 전투, 사이드 스크롤링, 플랫폼형 변형

## 주요 기능

### 1. 협동 전투 시스템
- 플레이어는 캐릭터 상태, 체력, 에너지, 스탯, 버프/디버프를 관리합니다.
- 기본 공격, 대쉬, 젬 스킬, 궁극기 발동 구조를 포함합니다.
- 팀 배치 역할(딜러, 탱커, 서포트 등)과 포메이션 로직이 구성되어 있습니다.

### 2. 웨이브 시스템
- `WaveManager`가 몬스터 스폰, 난이도 scaling, 보스 등장, 레벨 종료 흐름을 제어합니다.
- 각 웨이브마다 몬스터 수, 체력, 공격력, 보상 구조가 확장됩니다.
- 클리어/실패 시 결과 처리 및 다음 웨이브 전환 구조가 포함됩니다.

### 3. 젬/융합/업그레이드 시스템
- 플레이어는 젬을 수집하고 교환 및 융합을 수행할 수 있습니다.
- 스코어, 승패, 생존 시간, 보스 처치 결과를 기반으로 최종 등급을 계산합니다.
- 보상 시스템은 골드, 업그레이드 포인트, 퀘스트/게임 진행 지표를 반영합니다.

### 4. UI 및 로비 구성
- 메인 메뉴, 로비, HUD, 인벤토리 행 위젯 등 UMG 기반 UI가 포함됩니다.
- 게임 상태, 점수, 생존 시간, 몬스터 HP 표시, 보스 전투 정보 UI가 존재합니다.

### 5. 모드 변형 구조
- `Variant_Combat`
- `Variant_Platforming`
- `Variant_SideScrolling`

각각 다른 플레이 스타일을 가지는 하위 구현이 포함되어 있어, 공통 시스템을 재사용하면서 변형 게임플레이를 확장할 수 있습니다.

## 기술 스택

- Unreal Engine 5.7
- C++ Gameplay Framework
- UMG UI
- Enhanced Input
- StateTree / GameplayStateTree
- NavigationSystem / AIModule
- DataTable 기반 스테이터스 및 웨이브 데이터 관리

## 프로젝트 구조

```text
Project_GemCoop/
├── Config/                  # 프로젝트 설정
├── Content/                 # 에셋, 맵, 블루프린트, UI
├── Source/
│   └── Project_GemCoop/     # C++ 게임 모듈
│       ├── Variant_Combat/
│       ├── Variant_Platforming/
│       ├── Variant_SideScrolling/
│       ├── *.h / *.cpp      # 핵심 게임 시스템
│       └── Project_GemCoop.Build.cs
├── Binaries/
├── Intermediate/
├── Plugins/
├── Saved/
├── Project_GemCoop.uproject
├── Project_GemCoop.sln
├── README.md
└── .gitignore
```

## 핵심 파일

- `Project_GemCoop.uproject` : Unreal 프로젝트 진입 파일
- `Source/Project_GemCoop/Project_GemCoopCharacter.h` : 플레이어 캐릭터 기본 구조
- `Source/Project_GemCoop/Project_GemCoopGameMode.h` : 게임 흐름 및 결과 처리
- `Source/Project_GemCoop/Project_GemCoopWaveManager.h` : 웨이브 생성 및 스폰 관리
- `Source/Project_GemCoop/Project_GemCoopGemDataSubsystem.*` : 젬 데이터 관리
- `Source/Project_GemCoop/Project_GemCoopMonsterCharacter.*` : 몬스터 로직

## 요구사항

다음 환경을 권장합니다.

- Unreal Engine 5.7 이상
- Visual Studio 2022
- C++ 개발 워크로드 설치
- Windows 환경 권장

## 실행 방법

1. Unreal Engine 5.7 런처에서 프로젝트를 엽니다.
2. `Project_GemCoop.uproject` 파일을 열어 프로젝트를 로드합니다.
3. 필요한 경우 프로젝트 파일을 생성하거나 다시 생성합니다.
4. 에디터에서 원하는 맵을 열어 테스트합니다.
5. 필요한 기능을 개발/수정한 뒤 빌드 및 실행합니다.

## 개발 참고

- 공통 시스템은 C++ 모듈로 구현되며, 일부 UI와 로직은 블루프린트와 에셋으로 보완됩니다.
- `Content` 폴더에 맵, UI, 몬스터, 스폰 포인트, 데이터 테이블 등 주요 자산이 포함되어 있습니다.
- `Variant_*` 디렉터리를 통해 같은 게임의 다른 스타일 구현을 실험할 수 있습니다.

## 현재 프로젝트 성격

이 프로젝트는 단순한 데모가 아니라, Unreal Engine 기반의 게임 시스템 설계/실험 프로젝트에 가깝습니다. 특히 다음과 같은 기능 개발 연습에 적합합니다.

- 웨이브 시스템 설계
- 협동형 모드 로직
- 상태 관리 및 데이터 테이블 연동
- UI와 HUD 구조 설계
- 게임 루프와 점수 계산 로직
- C++와 블루프린트 결합 구조

## 라이선스

본 프로젝트는 개발 목적의 개인/실험 프로젝트이며, 별도 라이선스가 명시되지 않은 경우 프로젝트 소유자 기준으로 관리됩니다.

## 작성자/정보

- 프로젝트명: Project_GemCoop
- 엔진: Unreal Engine 5
- 주요 언어: C++
- 장르: 협동 액션 / 웨이브 서바이벌

필요하시면 다음 단계로 이어서 해드릴 수 있습니다.

- GitHub용 더 고급형 README 스타일로 다듬기
- 프로젝트 소개 문구를 한국어/영어로 분리하기
- 설치/실행 가이드 상세화
- 스크린샷 배너와 로고 영역 추가
- 라이선스 및 기여 가이드 문서 추가
