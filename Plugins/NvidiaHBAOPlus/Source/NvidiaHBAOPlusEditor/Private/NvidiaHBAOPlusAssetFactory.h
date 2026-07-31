#pragma once

#include "Factories/Factory.h"

#include "NvidiaHBAOPlusAssetFactory.generated.h"

UCLASS()
class UNvidiaHBAOPlusAssetFactory : public UFactory
{
	GENERATED_BODY()

public:
	UNvidiaHBAOPlusAssetFactory();

	virtual UObject* FactoryCreateNew(
		UClass* Class,
		UObject* InParent,
		FName Name,
		EObjectFlags Flags,
		UObject* Context,
		FFeedbackContext* Warn) override;

	virtual FText GetToolTip() const override;
};
