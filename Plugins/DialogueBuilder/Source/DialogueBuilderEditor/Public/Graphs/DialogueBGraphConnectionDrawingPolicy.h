// Copyright 2022 Greg Shynkar. All Rights Reserved

#pragma once

#include "Graphs/Nodes/EntryNodeDB.h"
#include "Graphs/Nodes/TransitionNodeDB.h"
#include "Graphs/Nodes/Slates/GraphNodeState.h"
#include "Graphs/Nodes/Slates/GraphNodeEntry.h"
#include "Graphs/Nodes/Slates/GraphNodeTransition.h"

#include "ConnectionDrawingPolicy.h"

class FDialogueBGraphConnectionDrawingPolicy : public FConnectionDrawingPolicy
{
public:
	FDialogueBGraphConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj);
	
	// FConnectionDrawingPolicy interface
	virtual void DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, /*inout*/ FConnectionParams& Params) override;
	virtual void Draw(TMap<TSharedRef<SWidget>, FArrangedWidget>& PinGeometries, FArrangedChildren& ArrangedNodes) override;
	virtual void DetermineLinkGeometry(FArrangedChildren& ArrangedNodes,TSharedRef<SWidget>& OutputPinWidget,UEdGraphPin* OutputPin,UEdGraphPin* InputPin, /*out*/ FArrangedWidget*& StartWidgetGeometry,/*out*/ FArrangedWidget*& EndWidgetGeometry) override;
	virtual void DrawSplineWithArrow(const FGeometry& StartGeom, const FGeometry& EndGeom, const FConnectionParams& Params) override;
	virtual void DrawSplineWithArrow(const FVector2D& StartPoint, const FVector2D& EndPoint, const FConnectionParams& Params) override;
	virtual void DrawPreviewConnector(const FGeometry& PinGeometry, const FVector2D& StartPoint, const FVector2D& EndPoint, UEdGraphPin* Pin) override;
	virtual FVector2D ComputeSplineTangent(const FVector2D& Start, const FVector2D& End) const override;
	// End of FConnectionDrawingPolicy interface

protected:
	void Internal_DrawLineWithArrow(const FVector2D& StartAnchorPoint, const FVector2D& EndAnchorPoint, const FConnectionParams& Params);


	UEdGraph* GraphObj;
	TMap<UEdGraphNode*, int32> NodeWidgetMap;
	
};
