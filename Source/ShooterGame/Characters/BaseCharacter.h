#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterGame/Actors/Interactive/InteractiveActor.h"
#include "ShooterGame/Components/MovementComponents/BaseCharacterMovementComponent.h"
#include "BaseCharacter.generated.h"

USTRUCT(BlueprintType)
struct FMantlingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* MantlingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* FPMantlingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveVector* MantlingCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, UIMin = 0.f))
	float AnimationCorrectionXY = 65.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, UIMin = 0.f))
	float AnimationCorrectionZ = 200.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, UIMin = 0.f))
	float MinHeight = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, UIMin = 0.f))
	float MaxHeight = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, UIMin = 0.f))
	float MinHeightStartTime = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, UIMin = 0.f))
	float MaxHeightStartTime = 0.f;
};

USTRUCT(BlueprintType)
struct FHardLandingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* HardLandingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, UIMin = 0.f), DisplayName = "Hard landing height")
	float HardLandingPlayHeight = 1000.f;
};

typedef TArray<AInteractiveActor*, TInlineAllocator<10>> TInteractiveActorsArray;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHardLandedSignature);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAimingStateChangedSignature, bool);

class UCharacterEquipmentComponent;
class UCharacterAttributesComponent;

UCLASS(Abstract, NotBlueprintable)
class SHOOTERGAME_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	explicit ABaseCharacter(const FObjectInitializer& ObjectInitializer);
	
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	virtual void MoveForward(const float Value) {}
	virtual void MoveRight(const float Value) {}
	
	virtual void Turn(float Value) {}
	virtual void LookUp(float Value) {};
	virtual void TurnAtRate(float Value) {};
	virtual void LookUpAtRate(float Value) {};

	virtual void ChangeCrouchState() {};
	
	virtual void StartSprint();
	virtual void EndSprint();

	virtual void SwimForward(float Value) {};
	virtual void SwimRight(float Value) {};
	virtual void SwimUp(float Value) {};

	void Mantle(bool Force = false);
	virtual bool CanMantle() const;
	virtual void OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime) {};

	virtual void InteractWithLadder();
	virtual void ClimbLadderUp(float Value);
	const ALadder* GetAvailableLadder() const;

	virtual void Zipline();
	virtual void StartZipline();
	virtual void EndZipline();
	virtual bool CanZipline();
	const AZipline* GetAvailableZipline() const;

	virtual void Falling() override;
	virtual void Landed(const FHitResult& Hit) override;

	UFUNCTION(BlueprintNativeEvent, Category = "Character")
	void OnHardLanded();
	virtual void OnHardLanded_Implementation();

	UPROPERTY(BlueprintAssignable, Category = "Character")
	FOnHardLandedSignature OnHardLandedDelegate;

	FOnAimingStateChangedSignature OnAimingStateChanged;
	
	FORCEINLINE UBaseCharacterMovementComponent* GetBaseCharacterMovementComponent() const { return BaseCharacterMovementComponent; }

	FORCEINLINE const UCharacterEquipmentComponent* GetCharacterEquipmentComponent() const { return CharacterEquipmentComponent; }

	FORCEINLINE UCharacterEquipmentComponent* GetCharacterEquipmentComponent_Mutable() const { return CharacterEquipmentComponent; }

	FORCEINLINE const UCharacterAttributesComponent* GetCharacterAttributesComponent() const { return CharacterAttributesComponent; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetIKRightFootOffset() const { return IKRightFootOffset; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetIKLeftFootOffset() const { return IKLeftFootOffset; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetIKPelvisOffset() const { return IKPelvisOffset; }

	void RegisterInteractiveActor(AInteractiveActor* InteractiveActor);
	
	void UnregisterInteractiveActor(AInteractiveActor* InteractiveActor);

	void StartFire() const;
	void StopFire() const;

	void StartAiming();
	void StopAiming();

	void ReloadEquippedWeapon() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BaseCharacter")
	void OnStartAiming();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BaseCharacter")
	void OnStopAiming();
	
	float GetAimingMovementSpeed() const;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsAiming() const { return bIsAiming; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Controls")
	float BaseTurnRate = 45.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Controls")
	float BaseLookUpRate = 45.f;

	UFUNCTION(BlueprintNativeEvent, Category = "Character|Movement")
	void OnSprintStart();
	virtual void OnSprintStart_Implementation() {};

	UFUNCTION(BlueprintNativeEvent, Category = "Character|Movement")
	void OnSprintEnd();
	virtual void OnSprintEnd_Implementation() {};
	virtual bool CanSprint();
	
	virtual bool CanJumpInternal_Implementation() const override;

	virtual void OnDeath();

	virtual void OnStartAimingInternal();
	virtual void OnStopAimingInternal();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Animations")
	UAnimMontage* OnDeathAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Attributes")
	UCurveFloat* FallDamageCurve;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Components")
	UBaseCharacterMovementComponent* BaseCharacterMovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|IK settings")
	FName RightFootSocketName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|IK settings")
	FName LeftFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|IK settings", meta = (ClampMin = 0.f, UIMin = 0.f))
	float IKTraceDistance = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|IK settings", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	// ReSharper disable once IdentifierTypo
	float IKInterpSpeed = 30.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Movement")
	class ULedgeDetectorComponent* LedgeDetectorComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Movement|Mantling")
	FMantlingSettings HighMantlingSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Movement|Mantling")
	FMantlingSettings LowMantlingSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Movement|Landing")
	FHardLandingSettings HardLandingSettings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Components")
	class UCharacterAttributesComponent* CharacterAttributesComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Movement|Mantling", meta = (ClampMin = 0.f, UIMin = 0.f))
	float LowMantleMaxHeight = 125.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Components")
	UCharacterEquipmentComponent* CharacterEquipmentComponent;
	
private:
	void TryChangeSprintState();
	bool IsSprintRequested = false;

	void UpdateIKSettings(float DeltaSeconds);
	float CalculateIKPelvisOffset() const;
	float CalculateIKOffsetForSocket(const FName& SocketName) const;
	
	float IKRightFootOffset = 0.f;
	float IKLeftFootOffset = 0.f;
	float IKPelvisOffset = 0.f;
	
	void OnStaminaHasChanged(bool InState);

	UFUNCTION()
	void OnReachedJumpApexHeight();
	float JumpApexHeight = 0.f;

	void PlayHardLandingMontage();
	void StopHardLandingMontage();

	void EnableRagdoll() const;

	const FMantlingSettings& GetMantlingSettings(float LedgeHeight) const;

	bool bCanSprint = true;
	bool bCanJump = true;
	bool bStaminaIsOver = false;
	bool bIsAiming = false;

	float AimingMovementSpeed = 0.f;
	
	// ReSharper disable once CppUE4ProbableMemoryIssuesWithUObjectsInContainer
	TInteractiveActorsArray InteractiveActors;
};
