# Project_GemTrio

<p align="center">
  <img src="https://img.shields.io/badge/Unreal%20Engine-5.7-4A90E2?style=for-the-badge&logo=unrealengine" alt="Unreal Engine 5.7" />
  <img src="https://img.shields.io/badge/C%2B%2B-Gameplay-00599C?style=for-the-badge&logo=c%2B%2B" alt="C++ Gameplay" />
  <img src="https://img.shields.io/badge/Genre-Co-op%20Action-FF6B6B?style=for-the-badge" alt="Co-op Action" />
  <img src="https://img.shields.io/badge/Status-Prototype%20Build-2ECC71?style=for-the-badge" alt="Prototype Build" />
</p>

Unreal Engine 5 기반의 협동 액션 / 웨이브 서바이벌 게임 프로토타입입니다. 캐릭터 전투, 스킬 루프, 보스전, 난이도 확장 구조, HUD/UI, 로비 흐름을 직접 설계하고 구현한 프로젝트입니다.

## Overview

이 프로젝트는 단순한 게임 데모가 아니라, 게임플레이 시스템 설계와 구현 역량을 보여주기 위한 포트폴리오 프로젝트입니다. 특히 다음을 중점적으로 다뤘습니다.

- 캐릭터 상태 관리와 전투 시스템 설계
- 협동형 플레이 흐름과 포메이션/역할 분담
- 웨이브 기반 스타트/클리어/실패 로직 설계
- 젬 기반 자원 루프와 업그레이드 구조
- HUD/UI 및 로비 → 게임 → 결과 흐름 구축
- 공통 시스템을 활용한 여러 게임 모드 확장 구조 설계

## Gameplay Features

### Character Combat System
- 3인칭 캐릭터 이동, 공격, 대시, 투사체 공격 구현
- 체력, 에너지, 버프/디버프, 상태 전환 구조 설계
- 스킬/궁극기 기반 시너지 흐름과 팀 전투 구조 반영
- 포메이션 역할을 고려한 전투 조작 경험 구현

### Wave Survival Loop
- `WaveManager`를 통해 스폰, 난이도 상승, 보스 등장, 클리어 조건 관리
- 타이머 기반 웨이브 진행과 다음 웨이브 전환 구조 설계
- DataTable 기반 스폰 데이터 구성으로 확장성 확보

### Gem & Progression System
- 젬 획득, 교환, 융합, 업그레이드 루프 구성
- 점수, 킬 수, 생존 시간, 보스 처치 수를 통합 계산하는 결과 시스템 설계
- 등급 산출을 통해 보상과 게임 성과를 연결

### UI / HUD / Game Flow
- 메인 메뉴, 로비, 인게임 HUD, 인벤토리 행 위젯 구현
- 몬스터 HP, 점수, 생존 시간, 결과 정보를 화면에 표시
- 로비 → 게임 → 결과 전환을 고려한 전체 흐름 설계

### Variant Architecture
- `Variant_Combat`
- `Variant_Platforming`
- `Variant_SideScrolling`

공통 시스템을 유지하면서 각기 다른 플레이 스타일을 확장하는 구조를 실험하고 있습니다. 이는 멀티모드 게임 설계와 재사용 가능한 시스템 아키텍처를 보여주는 핵심 포인트입니다.

## Tech Stack

- Unreal Engine 5.7
- C++ Gameplay Framework
- Enhanced Input
- UMG UI
- StateTree / GameplayStateTree
- AIModule / NavigationSystem
- DataTable 엔진 설계

## Project Structure

```text
Project_GemCoop/
├── Config/
├── Content/
│   ├── Maps/
│   ├── Blueprints/
│   ├── UI/
│   └── Data/
├── Source/
│   └── Project_GemCoop/
│       ├── Variant_Combat/
│       ├── Variant_Platforming/
│       ├── Variant_SideScrolling/
│       ├── Project_GemCoopCharacter.*
│       ├── Project_GemCoopGameMode.*
│       ├── Project_GemCoopWaveManager.*
│       ├── Project_GemCoopGemDataSubsystem.*
│       ├── Project_GemCoopMonsterCharacter.*
│       └── Project_GemCoop.Build.cs
├── Plugins/
├── Saved/
├── Project_GemCoop.uproject
├── Project_GemCoop.sln
├── README.md
└── .gitignore
```

## Key Systems

- `Project_GemCoop.uproject` — 프로젝트의 진입점
- `Project_GemCoopCharacter` — 캐릭터 이동, 상태, 전투 로직
- `Project_GemCoopGameMode` — 게임 루프, 승패, 점수, 결과 처리
- `Project_GemCoopWaveManager` — 웨이브 생성, 스폰, 클리어 판단
- `Project_GemCoopGemDataSubsystem` — 젬 및 리소스 데이터 관리
- `Project_GemCoopMonsterCharacter` — 몬스터 동작 및 전투 로직

## What I Focused On

- 상태 전환 및 게임 규칙 설계
- 기능별 C++ 모듈 분리와 확장성 고려
- DataTable 기반 설계와 시스템 결합
- 협동 플레이와 게임 루프의 자연스러운 흐름 설계
- UI/게임 로직의 연결 구조 설계
- 유지보수와 확장을 고려한 구조화된 코드 작성

## Development Highlights

- 공통 시스템 기반의 여러 게임 모드 확장 구조 구현
- 협동 전투와 웨이브 서바이벌 루프를 하나의 프로젝트로 통합
- 플레이어 스킬, 자원 관리, 난이도 제어를 함께 설계
- HUD와 로직 연결을 고려한 구조적인 게임 시스템 구성

## Requirements

- Unreal Engine 5.7
- Visual Studio 2022
- C++ 개발 워크로드 설치
- Windows 개발 환경 권장

## Run Guide

1. Unreal Engine 5.7 런처를 실행합니다.
2. 프로젝트 루트의 `Project_GemCoop.uproject` 파일을 엽니다.
3. 에디터에서 맵을 로드합니다.
4. 플레이 모드를 실행해 테스트합니다.

## Project Status

현재는 핵심 시스템 설계와 게임 루프 구현 단계에 가까운 프로토타입 프로젝트입니다. 게임플레이 구조와 확장 가능성을 검증하는 데 초점을 두고 있으며, 이후에는 밸런싱, UI polish, 멀티플레이 안정화, 기능 확장을 진행할 수 있습니다.

## Portfolio Value

이 프로젝트는 다음 역량을 보여줄 수 있습니다.

- Unreal Engine 5 C++ 기반 게임 시스템 구현
- 게임 루프 설계 및 시스템 통합
- 전투, 서바이벌, 스킬, 웨이브 설계 및 구현
- UI/상태 전환 설계
- 확장 가능한 모듈형 게임 아키텍처 설계

## License

본 프로젝트는 개인 포트폴리오 및 실험 목적의 프로젝트이며, 별도 라이선스가 명시되지 않은 경우 프로젝트 소유자 기준으로 관리됩니다.
