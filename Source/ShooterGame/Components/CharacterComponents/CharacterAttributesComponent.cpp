#include "CharacterAttributesComponent.h"

#include "ShooterGame/Characters/BaseCharacter.h"

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "ShooterGame/ShooterGameTypes.h"
#include "ShooterGame/Subsystems/DebugSubsystem.h"
#endif


UCharacterAttributesComponent::UCharacterAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCharacterAttributesComponent::BeginPlay()
{
	Super::BeginPlay();
	
	checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("UCharacterAttributesComponent can only be used with a ABaseCharacter owner"));

	CurrentHealth = MaxHealth;
	CurrentStamina = MaxStamina;
	CachedBaseCharacterOwner = StaticCast<ABaseCharacter*>(GetOwner());

	CachedBaseCharacterOwner->OnTakeAnyDamage.AddDynamic(this, &UCharacterAttributesComponent::OnTakeAnyDamage);
}

void UCharacterAttributesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	DrawDebugAttributes();
#endif
	UpdateStamina(DeltaTime);
}

void UCharacterAttributesComponent::UpdateStamina(float DeltaTime)
{
	const UBaseCharacterMovementComponent* BaseCharacterMovementComponent = CachedBaseCharacterOwner->GetBaseCharacterMovementComponent();
	if (BaseCharacterMovementComponent->IsSprinting() && BaseCharacterMovementComponent->Velocity.Size() > BaseCharacterMovementComponent->MaxWalkSpeed)
	{
		CurrentStamina -= StaminaConsumptionVelocity * DeltaTime;
	}
	else
	{
		CurrentStamina += StaminaRestoreVelocity * DeltaTime;
	}
	CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);

	if (OnOutOfStaminaEvent.IsBound())
	{
		if (FMath::IsNearlyZero(CurrentStamina))
		{
			OnOutOfStaminaEvent.Broadcast(true);
			
		}
		else if (CurrentStamina == MaxStamina)
		{
			OnOutOfStaminaEvent.Broadcast(false);
		}
	}
}

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
void UCharacterAttributesComponent::DrawDebugAttributes() const
{
	const UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	if (!DebugSubsystem->IsEnabledCategory(Debug::CharacterAttributes))
	{
		return;
	}

	const FVector TextLocation = CachedBaseCharacterOwner->GetActorLocation() + FVector(0.0f, 0.0f, 100.f);
	DrawDebugString(GetWorld(), TextLocation, FString::Printf(TEXT("Health: %.2f / %.2f"), CurrentHealth, MaxHealth), nullptr, FColor::Green, 0.f, true);
	DrawDebugString(GetWorld(), TextLocation - FVector(0.0f, 0.0f, 10.f), FString::Printf(TEXT("Stamina: %.2f / %.2f"), CurrentStamina, MaxStamina), nullptr, FColor::Blue, 0.f, true);
}
#endif

void UCharacterAttributesComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!IsValidDamage(Damage) || !IsAlive())
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);
	
	if (CurrentHealth <= 0.0f)
	{
		if (OnDeathEvent.IsBound())
		{
			OnDeathEvent.Broadcast();
		}
	}
}
