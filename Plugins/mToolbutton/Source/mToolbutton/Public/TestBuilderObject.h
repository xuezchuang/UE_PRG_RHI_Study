// Copyright 2022 Greg Shynkar. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/ObjectMacros.h"
#include "TestBuilderObject.generated.h"


UCLASS(BlueprintType, Blueprintable)
class UTestBuilderObject : public UActorComponent
{
	GENERATED_BODY()

public:
	UTestBuilderObject(const FObjectInitializer & ObjectInitializer);


};


