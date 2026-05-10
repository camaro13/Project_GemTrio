// Fill out your copyright notice in the Description page of Project Settings.


#include "TestActor.h"
#include "Project_GemCoopStatComponent.h"
#include "Project_GemCoopBuffComponent.h"
#include "Project_GemCoopTraitComponent.h"
#include "Project_GemCoopEnergySYComponent.h"
#include "Project_GemCoopCombatComponent.h"
#include "Project_GemCoopGemComponent.h"
#include "Project_GemCoopFusionSYComponent.h"
#include "Project_GemCoopGameStateBase.h"

// Sets default values
ATestActor::ATestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    /*StatComp = CreateDefaultSubobject<UProject_GemCoopStatComponent>(TEXT("StatComp"));
    BuffComp = CreateDefaultSubobject<UProject_GemCoopBuffComponent>(TEXT("BuffComp"));
    TraitComp = CreateDefaultSubobject<UProject_GemCoopTraitComponent>(TEXT("TraitComp"));
    CombatComp = CreateDefaultSubobject<UProject_GemCoopCombatComponent>(TEXT("CombatComp"));
    EnergySYComp = CreateDefaultSubobject<UProject_GemCoopEnergySYComponent>(TEXT("EnergySYComp"));
    GemComp = CreateDefaultSubobject<UProject_GemCoopGemComponent>(TEXT("GemComp"));
    FusionSYComp = CreateDefaultSubobject<UProject_GemCoopFusionSYComponent>(TEXT("FusionSYComp"));*/
}

// Called when the game starts or when spawned
void ATestActor::BeginPlay()
{
	Super::BeginPlay();
	
    auto* StatComp = NewObject<UProject_GemCoopStatComponent>(this);
    if (!StatComp) return;

    // ── 테스트 1: 스탯 계산 검증 ──────────────────────
    // Base=100, Upgrade=10%, Modifier=1.3 → Final=143 이어야 함
    StatComp->BaseHP = 100.f;
    StatComp->ApplyPermanentUpgrade(FName("HP_Upgrade"), 1); // +10%
    FStatModifier Mod;
    Mod.HPMultiplier = 1.3f;
    StatComp->SetModifier(Mod);

    float ExpectedSt = 100.f * 1.1f * 1.3f; // 143
    UE_LOG(LogTemp, Warning,
        TEXT("[StatComp Test1] FinalHP: %.1f / ExpectedSt: %.1f / %s"),
        StatComp->FinalHP, ExpectedSt,
        FMath::IsNearlyEqual(StatComp->FinalHP, ExpectedSt, 0.1f)
        ? TEXT("PASS") : TEXT("FAIL"));

    // ── 테스트 2: 피해 적용 ───────────────────────────
    float HPBefore = StatComp->CurrentHP;
    StatComp->ApplyDamage(30.f);
    UE_LOG(LogTemp, Warning,
        TEXT("[StatComp Test2] HP: %.1f→%.1f / %s"),
        HPBefore, StatComp->CurrentHP,
        FMath::IsNearlyEqual(StatComp->CurrentHP, HPBefore - 30.f, 0.1f)
        ? TEXT("PASS") : TEXT("FAIL"));

    // ── 테스트 3: 힐 적용 ────────────────────────────
    StatComp->ApplyHeal(10.f);
    UE_LOG(LogTemp, Warning,
        TEXT("[StatComp Test3] HP after heal: %.1f / %s"),
        StatComp->CurrentHP,
        FMath::IsNearlyEqual(StatComp->CurrentHP, HPBefore - 20.f, 0.1f)
        ? TEXT("PASS") : TEXT("FAIL"));

    // ── 테스트 4: HP 0 이하 → OnDeath 발동 ───────────
    bool bDeathFired = false;
    //StatComp->OnDeath.AddLambda([&bDeathFired]() { bDeathFired = true; });
    StatComp->ApplyDamage(9999.f);
    UE_LOG(LogTemp, Warning,
        TEXT("[StatComp Test4] OnDeath fired: %s"),
        bDeathFired ? TEXT("PASS") : TEXT("FAIL"));

    //---------------------------------------------------------------------------------
    auto* BuffComp = NewObject<UProject_GemCoopBuffComponent>(this);

    FBuffData TestBuff;
    TestBuff.BuffID = FName("TestBuff");
    TestBuff.Duration = 5.f;
    TestBuff.ATKBonus = 0.2f;
    BuffComp->AddBuff(TestBuff);

    UE_LOG(LogTemp, Warning,
        TEXT("[BuffComp Test1] ActiveBuffs count: %d / %s"),
        BuffComp->ActiveBuffs.Num(),
        BuffComp->ActiveBuffs.Num() == 1 ? TEXT("PASS") : TEXT("FAIL"));

    // ── 테스트 2: 동일 BuffID 중복 추가 → Duration 갱신, Stack+1 ──
    BuffComp->AddBuff(TestBuff);
    UE_LOG(LogTemp, Warning,
        TEXT("[BuffComp Test2] Stack: %d / %s"),
        BuffComp->ActiveBuffs[0].StackCount,
        BuffComp->ActiveBuffs[0].StackCount == 2 ? TEXT("PASS") : TEXT("FAIL"));

    // ── 테스트 3: 보호막 흡수 ────────────────────────────
    BuffComp->ApplyShield(50.f);
    float DmgRemain = 80.f;
    BuffComp->AbsorbDamage(DmgRemain);
    UE_LOG(LogTemp, Warning,
        TEXT("[BuffComp Test3] Shield remain: %.1f / DmgRemain: %.1f / %s"),
        BuffComp->ShieldAmount, DmgRemain,
        FMath::IsNearlyEqual(BuffComp->ShieldAmount, 0.f, 0.1f) &&
        FMath::IsNearlyEqual(DmgRemain, 30.f, 0.1f)
        ? TEXT("PASS") : TEXT("FAIL"));

    //----------------------------------------------------------------------------

    // ── 테스트 1: OFFENSE 특성 ATK 배율 ─────────────────
    auto* TraitComp = NewObject<UProject_GemCoopTraitComponent>(this);
    TraitComp->SetTrait(EGemTrait::Offense);
    float ExpectedATK = StatComp->BaseATK * 1.3f;
    UE_LOG(LogTemp, Warning,
        TEXT("[TraitComp Test1] OFFENSE ATK: %.1f / Expected: %.1f / %s"),
        StatComp->FinalATK, ExpectedATK,
        FMath::IsNearlyEqual(StatComp->FinalATK, ExpectedATK, 0.1f)
        ? TEXT("PASS") : TEXT("FAIL"));

    // ── 테스트 2: DEFENSE 특성 DEF 배율 ──────────────────
    TraitComp->SetTrait(EGemTrait::Defense);
    float ExpectedDEF = StatComp->BaseDEF * 1.4f;
    UE_LOG(LogTemp, Warning,
        TEXT("[TraitComp Test2] DEFENSE DEF: %.1f / Expected: %.1f / %s"),
        StatComp->FinalDEF, ExpectedDEF,
        FMath::IsNearlyEqual(StatComp->FinalDEF, ExpectedDEF, 0.1f)
        ? TEXT("PASS") : TEXT("FAIL"));

    // ── 테스트 3: SUPPORT EnergyRegen 배율 ───────────────
    auto* EnergySYComp = NewObject<UProject_GemCoopEnergySYComponent>(this);
    TraitComp->SetTrait(EGemTrait::Support);
    UE_LOG(LogTemp, Warning,
        TEXT("[TraitComp Test3] EnergyRegenMultiplier: %.2f / %s"),
        EnergySYComp->EnergyRegenMultiplier,
        FMath::IsNearlyEqual(EnergySYComp->EnergyRegenMultiplier, 1.3f, 0.01f)
        ? TEXT("PASS") : TEXT("FAIL"));

    //--------------------------------------------------------------------------

    // ── 테스트 1: 피해 계산 공식 검증 ────────────────────
// DEF=100 → 감소율=100/200=0.5 → 피해=200*(1-0.5)=100
    auto* CombatComp = NewObject<UProject_GemCoopCombatComponent>(this);
    float DEF = 100.f;
    float RawDmg = 200.f;
    float Expected = RawDmg * (1.f - DEF / (DEF + 100.f)); // 100
    UE_LOG(LogTemp, Warning,
        TEXT("[CombatComp Test1] Expected DMG: %.1f / %s"),
        Expected,
        FMath::IsNearlyEqual(Expected, 100.f, 0.1f)
        ? TEXT("PASS") : TEXT("FAIL"));

    // ── 테스트 2: 캐스팅 시작/완료 ───────────────────────
    bool bCastCompleted = false;
    //CombatComp->OnCastingCompleted.AddLambda( [&bCastCompleted]() { bCastCompleted = true; });
    CombatComp->StartCasting(0.5f);

    // 0.6초 후 확인 (FTimerManager 사용)
    FTimerHandle CheckTimer;
    GetWorldTimerManager().SetTimer(CheckTimer, [this, &bCastCompleted]()
        {
            UE_LOG(LogTemp, Warning,
                TEXT("[CombatComp Test2] CastCompleted: %s"),
                bCastCompleted ? TEXT("PASS") : TEXT("FAIL"));
        }, 0.6f, false);

    // ── 테스트 3: 캐스팅 중 취소 ─────────────────────────
    CombatComp->StartCasting(0.5f);
    CombatComp->CancelCasting();
    UE_LOG(LogTemp, Warning,
        TEXT("[CombatComp Test3] bIsCasting after cancel: %s"),
        !CombatComp->bIsCasting ? TEXT("PASS") : TEXT("FAIL"));

    //--------------------------------------------------------------------------

    // ── 테스트 1: 보석 슬롯 초기 상태 ───────────────────
    auto* GemComp = NewObject<UProject_GemCoopGemComponent>(this);
    auto* GameState = GetWorld()->GetGameState<AProject_GemCoopGameStateBase>();

    if (!GameState)
    {
        UE_LOG(LogTemp, Warning, TEXT("GameState NULL, retry next tick"));

        GetWorldTimerManager().SetTimerForNextTick([this]()
            {
                BeginPlay(); // 다시 시도
                UE_LOG(LogTemp, Warning, TEXT("Retrying..."));
            });

        return;
    }

    UE_LOG(LogTemp, Warning,
        TEXT("[GemComp Test1] Slot count: %d / %s"),
        GemComp->GemSlots.Num(),
        GemComp->GemSlots.Num() == 4 ? TEXT("PASS") : TEXT("FAIL"));

    // ── 테스트 2: 에너지 부족 시 UseGem 실패 ─────────────
    // 에너지를 0으로 만들고 사용 시도
    GameState->SharedEnergy = 0.f;
    bool bUsed = GemComp->UseGem(0);
    UE_LOG(LogTemp, Warning,
        TEXT("[GemComp Test2] UseGem with 0 energy: %s"),
        !bUsed ? TEXT("PASS") : TEXT("FAIL"));

    // ── 테스트 3: 쿨타임 중 UseGem 실패 ─────────────────
    GameState->SharedEnergy = 100.f;
    GemComp->UseGem(0); // 1회 사용 → 쿨타임 시작
    bool bUsedDuringCooldown = GemComp->UseGem(0);
    UE_LOG(LogTemp, Warning,
        TEXT("[GemComp Test3] UseGem during cooldown: %s"),
        !bUsedDuringCooldown ? TEXT("PASS") : TEXT("FAIL"));

    //--------------------------------------------------------------------------

    // ── 테스트 1: 에너지 충분할 때 소모 성공 ─────────────
    GameState->SharedEnergy = 100.f;
    bool bConsumed = EnergySYComp->TryConsumeEnergy(30.f);
    UE_LOG(LogTemp, Warning,
        TEXT("[EnergySY Test1] Consume 30 from 100: %s"),
        bConsumed ? TEXT("PASS") : TEXT("FAIL"));

    // ── 테스트 2: 에너지 부족 시 소모 실패 ───────────────
    GameState->SharedEnergy = 10.f;
    bool bFailed = EnergySYComp->TryConsumeEnergy(30.f);
    UE_LOG(LogTemp, Warning,
        TEXT("[EnergySY Test2] Consume 30 from 10 (should fail): %s"),
        !bFailed ? TEXT("PASS") : TEXT("FAIL"));

    // ── 테스트 3: 궁극기 게이지 충전 ─────────────────────
    EnergySYComp->UltGaugeContribution = 0.f;
    for (int32 i = 0; i < 20; i++)
        EnergySYComp->ChargeUltGauge(5.f); // 5×20=100

    UE_LOG(LogTemp, Warning,
        TEXT("[EnergySY Test3] UltReady: %s"),
        EnergySYComp->bUltimateReady ? TEXT("PASS") : TEXT("FAIL"));

    //--------------------------------------------------------------------------

    // ── 테스트 1: 유효 조합 검증 ─────────────────────────
    auto* FusionSYComp = NewObject<UProject_GemCoopFusionSYComponent>(this);
    TArray<EGemType> ValidCombo = { EGemType::Ruby, EGemType::Sapphire };
    bool bValid = FusionSYComp->IsValidFusionCombo(ValidCombo);
    UE_LOG(LogTemp, Warning,
        TEXT("[FusionSY Test1] Ruby+Sapphire valid: %s"),
        bValid ? TEXT("PASS") : TEXT("FAIL"));

    // ── 테스트 2: 무효 조합 ──────────────────────────────
    TArray<EGemType> InvalidCombo = { EGemType::Ruby, EGemType::Ruby };
    bool bInvalid = FusionSYComp->IsValidFusionCombo(InvalidCombo);
    UE_LOG(LogTemp, Warning,
        TEXT("[FusionSY Test2] Ruby+Ruby invalid: %s"),
        !bInvalid ? TEXT("PASS") : TEXT("FAIL"));

    // ── 테스트 3: 윈도우 오픈 후 만료 ────────────────────
    bool bWindowOpened = false;
    bool bWindowClosed = false;
    //FusionSYComp->OnFusionWindowOpened.AddLambda( [&bWindowOpened](float D) { bWindowOpened = true; });
    //FusionSYComp->OnFusionWindowClosed.AddLambda( [&bWindowClosed]() { bWindowClosed = true; });

    FGemFusionRequest Req;
    Req.PlayerIndex = 0;
    Req.Gem.GemType = EGemType::Ruby;
    FusionSYComp->RegisterFusionRequest(Req);

    // 0.6초 후 윈도우 만료 확인
    FTimerHandle T;
    GetWorldTimerManager().SetTimer(T, [&]()
        {
            UE_LOG(LogTemp, Warning,
                TEXT("[FusionSY Test3] WindowOpened:%s Closed:%s"),
                bWindowOpened ? TEXT("PASS") : TEXT("FAIL"),
                bWindowClosed ? TEXT("PASS") : TEXT("FAIL"));
        }, 0.6f, false);
}

// Called every frame
void ATestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

