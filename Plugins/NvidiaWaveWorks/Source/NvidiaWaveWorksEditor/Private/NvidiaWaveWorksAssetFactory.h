#pragma once

#include "Factories/Factory.h"

#include "NvidiaWaveWorksAssetFactory.generated.h"

UCLASS()
class UNvidiaWaveWorksAssetFactory : public UFactory
{
	GENERATED_BODY()

public:
	UNvidiaWaveWorksAssetFactory();

	virtual UObject* FactoryCreateNew(
		UClass* Class,
		UObject* InParent,
		FName Name,
		EObjectFlags Flags,
		UObject* Context,
		FFeedbackContext* Warn) override;

	virtual FText GetToolTip() const override;
};
