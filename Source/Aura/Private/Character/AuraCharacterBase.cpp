// Copyright Lucas Wang


#include "Character/AuraCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AuraConstants.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"
#include "DataTable/Combat/MontageWeaponRow.h"
#include "UI/Widget/DamageTextComponent.h"
#include "Util/AuraUtils.h"

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), "WeaponHandSocket");
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes);
	ApplyEffectToSelf(DefaultSecondaryAttributes);

	// Vital Attributes (Health and Mana) are dependent on Secondary Attributes (Max Health and Max Mana)
	ApplyEffectToSelf(DefaultVitalAttributes);
}

void AAuraCharacterBase::ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& GameplayEffectClass, float Level) const
{
	check(IsValid(AbilitySystemComponent));
	check(GameplayEffectClass);
	
	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}


void AAuraCharacterBase::Die()
{
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleDeath();
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation()
{
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Dissolve();

	bDead = true;
}

void AAuraCharacterBase::MulticastShowDamageNumber_Implementation(const float DamageAmount, bool bBlockedHit, bool bCriticalHit)
{
	if (DamageTextComponentClass)
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(this, DamageTextComponentClass, NAME_None, RF_Transient);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
	}
}

void AAuraCharacterBase::Dissolve()
{
	if (IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMatInst);
		StartDissolveTimeline(DynamicMatInst);
	}
	if (IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, DynamicMatInst);
		StartWeaponDissolveTimeline(DynamicMatInst);
	}
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	if (const FMontageWeaponRow* Row = FAuraUtils::GetDataTableRow<FMontageWeaponRow>(this,
		FName(DataTableName::MontageWeapon), MontageTag.GetTagName()))
	{
		const FName SocketName = FAuraUtils::GetFNamePropertyValue(this, Row->WeaponSocketProperty);
		if (SocketName == NAME_None)
		{
			UE_LOG(LogTemp, Warning, TEXT("Property %s not found in class %s: 在 %s 表中配置错误"), *Row->WeaponSocketProperty.ToString(),
			       *GetClass()->GetName(), *DataTableName::MontageWeapon.ToString());
		}

		// 位置要么在 Mesh 上，要么在 Weapon 上
		if (GetMesh()->DoesSocketExist(SocketName))
		{
			return GetMesh()->GetSocketLocation(SocketName);
		}
		if (Weapon && Weapon->DoesSocketExist(SocketName))
		{
			return Weapon->GetSocketLocation(SocketName);
		}
	}
	return FVector();
}
	

bool AAuraCharacterBase::IsDead_Implementation() const
{
	return bDead;
}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

