// Copyright Lucas Wang

#pragma once

#include "CoreMinimal.h"


#pragma region Delegate

template<class T>
using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;

#pragma endregion 


#pragma region Const

#define ECC_Projectile ECollisionChannel::ECC_GameTraceChannel1

#pragma endregion