// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueSystemTypes.h"
#include "DialogueSymbolWidget.generated.h"

class UImage;
class UWidgetAnimation;

DECLARE_DYNAMIC_DELEGATE_OneParam(FDialogueSymbolEvent, FString, SymbolID);

/**
 * 
 */
UCLASS()
class DIALOGUESYSTEM_API UDialogueSymbolWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	FDialogueSymbolEvent OnSelectedEvent;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* SymbolImage = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* GlowImage = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* FrameImage = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetAnim))
		UWidgetAnimation* GlowLoopingAnim;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetAnim))
		UWidgetAnimation* GlowSymbolAnim;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetAnim))
		UWidgetAnimation* FrameVisibilityAnim;

	void SetDialogueSymbolData(FDialogueSymbolData NewData);

	FDialogueSymbolData GetSymbolData() const;

	void StartOnFocusAnim();

	void EndOnFocusAnim();

protected:

	UPROPERTY(BlueprintReadWrite, Category = "DialogueSymbolWidget")
		FDialogueSymbolData DialogueSymbolData;

	void NativeConstruct() override;

	FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

	void NativeOnFocusLost(const FFocusEvent& InFocusEvent);

	FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
};
