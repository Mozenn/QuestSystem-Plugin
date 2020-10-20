#include "SDialogueGraphEdge.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SToolTip.h"
#include "SGraphPanel.h"
#include "DialogueGraphConnectionDrawingPolicy.h"
#include "DialogueEdEdge.h"
#include "DialogueEdNode.h"

#define LOCTEXT_NAMESPACE "SDialogueGraphEdge"

void SDialogueGraphEdge::Construct(const FArguments& InArgs, UDialogueEdEdge* InNode)
{
	this->GraphNode = InNode;
	this->UpdateGraphNode();
}

bool SDialogueGraphEdge::RequiresSecondPassLayout() const
{
	return true;
}

void SDialogueGraphEdge::PerformSecondPassLayout(const TMap< UObject*, TSharedRef<SNode> >& NodeToWidgetLookup) const
{
	UDialogueEdEdge* EdgeNode = CastChecked<UDialogueEdEdge>(GraphNode);

	FGeometry StartGeom;
	FGeometry EndGeom;

	UDialogueEdNode* Start = EdgeNode->GetStartNode();
	UDialogueEdNode* End = EdgeNode->GetEndNode();
	if (Start != nullptr && End != nullptr)
	{
		const TSharedRef<SNode>* pFromWidget = NodeToWidgetLookup.Find(Start);
		const TSharedRef<SNode>* pToWidget = NodeToWidgetLookup.Find(End);
		if (pFromWidget != nullptr && pToWidget != nullptr)
		{
			const TSharedRef<SNode>& FromWidget = *pFromWidget;
			const TSharedRef<SNode>& ToWidget = *pToWidget;

			StartGeom = FGeometry(FVector2D(Start->NodePosX, Start->NodePosY), FVector2D::ZeroVector, FromWidget->GetDesiredSize(), 1.0f);
			EndGeom = FGeometry(FVector2D(End->NodePosX, End->NodePosY), FVector2D::ZeroVector, ToWidget->GetDesiredSize(), 1.0f);
		}
	}

	PositionBetweenTwoNodesWithOffset(StartGeom, EndGeom, 0, 1);
}

void SDialogueGraphEdge::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
		[
			SNew(SImage)
			.Image(FEditorStyle::GetBrush("Graph.TransitionNode.ColorSpill"))
		.ColorAndOpacity(this, &SDialogueGraphEdge::GetEdgeColor)
		]
	+ SOverlay::Slot()
		[
			SNew(SImage)
			.Image(FEditorStyle::GetBrush("Graph.TransitionNode.Icon"))
		]
		];
}

void SDialogueGraphEdge::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	UDialogueEdEdge* EdgeNode = CastChecked<UDialogueEdEdge>(GraphNode);
	if (UEdGraphPin* Pin = EdgeNode->GetInputPin())
	{
		GetOwnerPanel()->AddPinToHoverSet(Pin);
	}

	SGraphNode::OnMouseEnter(MyGeometry, MouseEvent);
}

void SDialogueGraphEdge::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	UDialogueEdEdge* EdgeNode = CastChecked<UDialogueEdEdge>(GraphNode);
	if (UEdGraphPin* Pin = EdgeNode->GetInputPin())
	{
		GetOwnerPanel()->RemovePinFromHoverSet(Pin);
	}

	SGraphNode::OnMouseLeave(MouseEvent);
}

void SDialogueGraphEdge::PositionBetweenTwoNodesWithOffset(const FGeometry& StartGeom, const FGeometry& EndGeom, int32 NodeIndex, int32 MaxNodes) const
{
	// Get a reasonable seed point (halfway between the boxes)
	const FVector2D StartCenter = FGeometryHelper::CenterOf(StartGeom);
	const FVector2D EndCenter = FGeometryHelper::CenterOf(EndGeom);
	const FVector2D SeedPoint = (StartCenter + EndCenter) * 0.5f;

	// Find the (approximate) closest points between the two boxes
	const FVector2D StartAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(StartGeom, SeedPoint);
	const FVector2D EndAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(EndGeom, SeedPoint);

	// Position ourselves halfway along the connecting line between the nodes, elevated away perpendicular to the direction of the line
	const float Height = 30.0f;

	const FVector2D DesiredNodeSize = GetDesiredSize();

	FVector2D DeltaPos(EndAnchorPoint - StartAnchorPoint);

	if (DeltaPos.IsNearlyZero())
	{
		DeltaPos = FVector2D(10.0f, 0.0f);
	}

	const FVector2D Normal = FVector2D(DeltaPos.Y, -DeltaPos.X).GetSafeNormal();

	const FVector2D NewCenter = StartAnchorPoint + (0.5f * DeltaPos) + (Height * Normal);

	FVector2D DeltaNormal = DeltaPos.GetSafeNormal();

	// Calculate node offset in the case of multiple transitions between the same two nodes
	// MultiNodeOffset: the offset where 0 is the centre of the transition, -1 is 1 <size of node>
	// towards the PrevStateNode and +1 is 1 <size of node> towards the NextStateNode.

	const float MutliNodeSpace = 0.2f; // Space between multiple transition nodes (in units of <size of node> )
	const float MultiNodeStep = (1.f + MutliNodeSpace); //Step between node centres (Size of node + size of node spacer)

	const float MultiNodeStart = -((MaxNodes - 1) * MultiNodeStep) / 2.f;
	const float MultiNodeOffset = MultiNodeStart + (NodeIndex * MultiNodeStep);

	// Now we need to adjust the new center by the node size, zoom factor and multi node offset
	const FVector2D NewCorner = NewCenter - (0.5f * DesiredNodeSize) + (DeltaNormal * MultiNodeOffset * DesiredNodeSize.Size());

	GraphNode->NodePosX = NewCorner.X;
	GraphNode->NodePosY = NewCorner.Y;
}


FSlateColor SDialogueGraphEdge::GetEdgeColor() const
{
	const FLinearColor HoverColor(0.724f, 0.256f, 0.0f, 1.0f);
	FLinearColor BaseColor(0.9f, 0.9f, 0.9f, 1.0f);

	return IsHovered() ? HoverColor : BaseColor;
}

#undef LOCTEXT_NAMESPACE