// Copyright 2022 Greg Shynkar. All Rights Reserved

#pragma once

#include "DialogueBuilderEdTabFactories.h"
#include "DialogueBuilderCustomEditor.h"
#include "CoreMinimal.h"
#include "BlueprintEditorTabs.h"
#include "BlueprintEditorModes.h"

class DIALOGUEBUILDEREDITOR_API FDialogueBuilderApplicationMode : public FBlueprintEditorApplicationMode
{
public:
	// Interface of FBlueprintEditorApplicationMode
	FDialogueBuilderApplicationMode(TSharedPtr<class FDialogueBuilderCustomEditor> InDialogueBuilderEditor);
	//FDialogueBuilderApplicationMode(class FDialogueBuilderCustomEditor* InDialogueBuilderEditor);
	virtual ~FDialogueBuilderApplicationMode();
	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void PostActivateMode() override;
protected:
	TWeakPtr<FDialogueBuilderCustomEditor> DialogueBuilderEditor;
	FDialogueBuilderCustomEditor* InDialogueBuilderEditorRef;
	//TSharedPtr<FDialogueBuilderCustomEditor> DialogueBuilderEditor;
	FWorkflowAllowedTabSet StardardTabFactories;
};
