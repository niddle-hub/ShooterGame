#include "CharacterAttributesComponent.h"

#include "GameFramework/PhysicsVolume.h"
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

	SetHealth(MaxHealth);
	SetStamina(MaxStamina);
	SetOxygen(MaxOxygen);
	
	CachedBaseCharacterOwner = StaticCast<ABaseCharacter*>(GetOwner());

	CachedBaseCharacterOwner->OnTakeAnyDamage.AddDynamic(this, &UCharacterAttributesComponent::OnTakeAnyDamage);
	OnOutOfOxygenEvent.AddUObject(this, &UCharacterAttributesComponent::OnOxygenHasChanged);
}

void UCharacterAttributesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	DrawDebugAttributes();
#endif
	UpdateStamina(DeltaTime);
	UpdateOxygen(DeltaTime);
}

void UCharacterAttributesComponent::SetHealth(float NewHealth)
{
	CurrentHealth = FMath::Clamp(NewHealth, 0.f, MaxHealth);
	if (OnHealthChangedEvent.IsBound())
	{
		OnHealthChangedEvent.Broadcast(CurrentHealth);
	}
}

void UCharacterAttributesComponent::SetStamina(float NewStamina)
{
	CurrentStamina = FMath::Clamp(NewStamina, 0.f, MaxStamina);
	if (OnStaminaChangedEvent.IsBound())
	{
		OnStaminaChangedEvent.Broadcast(CurrentStamina);
	}
}

void UCharacterAttributesComponent::SetOxygen(float NewOxygen)
{
	CurrentOxygen = FMath::Clamp(NewOxygen, 0.f, MaxOxygen);
	if (OnOxygenChangedEvent.IsBound())
	{
		OnOxygenChangedEvent.Broadcast(CurrentOxygen);
	}
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
	
	SetStamina(CurrentStamina);
	
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

void UCharacterAttributesComponent::UpdateOxygen(float DeltaTime)
{
	const UBaseCharacterMovementComponent* BaseCharacterMovementComponent = CachedBaseCharacterOwner->GetBaseCharacterMovementComponent();
	
	const FVector HeadPosition = CachedBaseCharacterOwner->GetMesh()->GetSocketLocation(FName("Head"));
	const APhysicsVolume* Volume = BaseCharacterMovementComponent->GetPhysicsVolume();
	const float VolumeTopPlane = Volume->GetActorLocation().Z + Volume->GetBounds().BoxExtent.Z * Volume->GetActorScale3D().Z;
	
	if (BaseCharacterMovementComponent->IsSwimming() && VolumeTopPlane > HeadPosition.Z)
	{
		CurrentOxygen -= OxygenConsumptionVelocity * DeltaTime;
	}
	else
	{
		CurrentOxygen += OxygenRestoreVelocity * DeltaTime;
	}
	
	SetOxygen(CurrentOxygen);
	
	if (OnOutOfOxygenEvent.IsBound())
	{
		OnOutOfOxygenEvent.Broadcast(FMath::IsNearlyZero(CurrentOxygen));
	}
	
}

void UCharacterAttributesComponent::OnOxygenHasChanged(const bool InState)
{
	if (InState && !GetWorld()->GetTimerManager().IsTimerActive(OxygenDamageTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(OxygenDamageTimerHandle, this, &UCharacterAttributesComponent::TakeOxygenDamage, OxygenDamageDelay, false);
	}
	else if (!InState && GetWorld()->GetTimerManager().IsTimerActive(OxygenDamageTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(OxygenDamageTimerHandle);
	}
}

void UCharacterAttributesComponent::TakeOxygenDamage() const
{
	APhysicsVolume* VolumeDamageCauser = CachedBaseCharacterOwner->GetBaseCharacterMovementComponent()->GetPhysicsVolume();
	CachedBaseCharacterOwner->TakeDamage(OxygenDamage, FDamageEvent(), CachedBaseCharacterOwner->GetController(), VolumeDamageCauser);
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
	DrawDebugString(GetWorld(), TextLocation - FVector(0.0f, 0.0f, 10.f), FString::Printf(TEXT("Stamina: %.2f / %.2f"), CurrentStamina, MaxStamina), nullptr, FColor::Orange, 0.f, true);
	DrawDebugString(GetWorld(), TextLocation - FVector(0.0f, 0.0f, 20.f), FString::Printf(TEXT("Oxygen: %.2f / %.2f"), CurrentOxygen, MaxOxygen), nullptr, FColor::Cyan, 0.f, true);
}
#endif

void UCharacterAttributesComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!IsValidDamage(Damage) || !IsAlive())
	{
		return;
	}

	SetHealth(CurrentHealth - Damage);
	
	if (CurrentHealth <= 0.0f)
	{
		if (OnDeathEvent.IsBound())
		{
			OnDeathEvent.Broadcast();
		}
	}
}
