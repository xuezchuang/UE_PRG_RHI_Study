// Copyright 2022 Greg Shynkar. All Rights Reserved

#pragma once

#include "Animation/AnimSequence.h"
#include "Net/UnrealNetwork.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/ObjectMacros.h"
#include "DialogueBuilderObject.generated.h"

// Structure To keep data from Dialogue Node;
USTRUCT(BlueprintType)
struct DIALOGUEBUILDER_API FDialogueDetailsStruct
{
	GENERATED_USTRUCT_BODY()

	// Visible Data Parts in Dialogue Editor Details Tab;
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue Details Struct")
	int32 NodeId;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue Details Struct")
	FString DialogueNodeText;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue Details Struct")
	bool IsPlayerNode;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue Details Struct")
	bool IsPlayerAnswerOption;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue Details Struct")
	FString SpeakerName;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue Details Struct")
	UTexture2D* SpeakerTexture = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue Details Struct")
	bool IsSwitchNode;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue Details Struct")
	float DelayToMoveNextNodes;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue Details Struct")
	class USoundBase* DialogueAudio = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue Details Struct")
	class UDialogueWave* DialogueWave = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue Details Struct")
	float BeginPlayDelay;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue Details Struct")
	bool IsAlreadyPlayed=false;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue Details Struct")
	UAnimSequence* AnimToUpdate= nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue Details Struct")
	bool IsItEndNode;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue Details Struct")
	bool WriteEffect;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue Details Struct")
	float WriteEffectSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue Details Struct")
	bool IsTimedDecisions;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue Details Struct")
	float TimedDecisionsTime;

	// Non-Visible Working Data Parts in Dialogue Editor;
	UPROPERTY()
	FVector2D NodePosition;

	UPROPERTY()
	TArray <int32> NextNodesId;

	UPROPERTY()
	TArray <FName> TransitionNameToCheck;

	UPROPERTY()
	FName OnDialoguePlayFuncName;

	bool operator==(const FDialogueDetailsStruct & Other) const
	{
		if (NodeId== Other.NodeId && DialogueNodeText== Other.DialogueNodeText)
		{
			return true;
		}
		return false;
	}
};


UCLASS(BlueprintType)
class DIALOGUEBUILDER_API UDialogueBuilderObject : public UActorComponent
{
	GENERATED_BODY()

public:
	UDialogueBuilderObject(const FObjectInitializer & ObjectInitializer);

	// Used for check Duplication of File to prevent same UEGraph ID;
	virtual void PostDuplicate(bool bDuplicateForPIE) override;

	// Start Dialogue From Node Connected to "Start Dialogue" Node;
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Start Dialogue"), Category = "Dialogue Builder")
	FDialogueDetailsStruct StartDialogue();

	// Start Dialogue From Nodes Connected to DialogueNode;
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Start Dialogue From Node ID"), Category = "Dialogue Builder")
	TArray<FDialogueDetailsStruct> StartDialogueFromNodeID(int32 NodeID);

	// Move To Next Nodes Connected to DialogueNode;
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Move To Next Nodes From Node ID"), Category = "Dialogue Builder")
	TArray<FDialogueDetailsStruct> MoveToNextNodesFromNodeID(int32 CurrentNodeID);

	// Check if Node is already played;
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Mark Node as Already Played"), Category = "Dialogue Builder")
	bool MarkNodeasAlreadyPlayed(int32 NodeID);

	// Search Dialogue Node inside DataArray and return Index;
	UFUNCTION()
	int32 GetListIndexFromNodeID(int32 NodeID);

	// Check Transition Function if possible to move from node to node;
	UFUNCTION()
	bool IsPossibleToMoveFurther(int32 DataElementIndex, int32 FunctionNameIndex);

	// Add New Element to Dialogue Data Array; 
	UFUNCTION()
	void AddDialogueDataElement(FDialogueDetailsStruct DialoguesElement);
	
	// Remove Element from Dialogue Data Array; 
	UFUNCTION()
	bool RemoveDataElement(int32 ArrayElementIndex);

	// Set Element at Dialogue Data Array; 
	UFUNCTION()
	bool SetDataElement(int32 DataElementNumber, FDialogueDetailsStruct NewElement);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateAnimationForDialogue(FDialogueDetailsStruct DialogueNodeDetails);

	// Debug Function for check data inside DialoguesData;
	UFUNCTION()
	void DebugGetAllIndexes();

	// Array to keep all data from Dialogue Graph Tab;
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Dialogue Builder")
	TArray <FDialogueDetailsStruct> DialoguesData;

	UPROPERTY()
	bool IsJustDuplicated=false;

	UPROPERTY(Replicated)
	float Test;
};


