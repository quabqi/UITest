#pragma once
#include "CoreMinimal.h"
#include "SInventoryWidget.h"

#include "Components/Widget.h"

#include "InventoryWidget.generated.h"

USTRUCT(BlueprintType)
struct FInventoryInstanceData
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 IconIndex = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Count = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Quality = 0;
};

UCLASS()
class UInventoryWidget : public UWidget
{
    GENERATED_BODY()

public:
    /** Image to draw */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
    FSlateBrush Brush;

    /** A bindable delegate for the Image. */
    UPROPERTY()
    FGetSlateBrush BrushDelegate;

    /** Color and opacity */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance, meta=( sRGB="true") )
    FLinearColor ColorAndOpacity;

    /** A bindable delegate for the ColorAndOpacity. */
    UPROPERTY()
    FGetLinearColor ColorAndOpacityDelegate;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
    TArray<FInventoryInstanceData> InstanceData;

    /** How many pieces there are */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance, meta=( ClampMin="1", ClampMax="25", UIMin="1", UIMax="25" ))
    int32 NumberOfColumns = 4;
    
    /** How many pieces there are */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance, meta=( ClampMin="1", ClampMax="25", UIMin="1", UIMax="25" ))
    int32 NumberOfRows = 4;
    
    /** How many pieces there are */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance, meta=( ClampMin="1", ClampMax="25", UIMin="1", UIMax="25" ))
    FVector2D DesiredSizeOverride = FVector2D(600, 600);
public:
    /**  */
    UFUNCTION(BlueprintCallable, Category="Appearance")
    void SetColorAndOpacity(FLinearColor InColorAndOpacity);

    /**  */
    UFUNCTION(BlueprintCallable, Category="Appearance")
    void SetOpacity(float InOpacity);

    UFUNCTION(BlueprintCallable, Category = "Appearance")
    void SetDesiredSizeOverride(FVector2D DesiredSize);

    /** Sets how many columns there are */
    UFUNCTION(BlueprintCallable, Category="Appearance")
    void SetNumberOfColumns(int32 InNumberOfColumns);
    
    /** Sets how many rows there are */
    UFUNCTION(BlueprintCallable, Category="Appearance")
    void SetNumberOfRows(int32 InNumberOfRows);
    
    /** Translates the bound brush data and assigns it to the cached brush used by this widget. */
    const FSlateBrush* ConvertImage(TAttribute<FSlateBrush> InImageAsset) const;

    /**  */
    UFUNCTION(BlueprintCallable, Category="Appearance")
    UMaterialInstanceDynamic* GetDynamicMaterial();
    
    UFUNCTION(BlueprintCallable, Category="Appearance")
    void UpdateMaterialParameterValue();
    
    //~ Begin UWidget Interface
    virtual void SynchronizeProperties() override;
    //~ End UWidget Interface

    //~ Begin UVisual Interface
    virtual void ReleaseSlateResources(bool bReleaseChildren) override;
    //~ End UVisual Interface

    #if WITH_EDITOR
    //~ Begin UWidget Interface
    virtual const FText GetPaletteCategory() override;
    //~ End UWidget Interface
    #endif

protected:
    //~ Begin UWidget Interface
    virtual TSharedRef<SWidget> RebuildWidget() override;
    //~ End UWidget Interface
private:
    TSharedPtr<SInventoryWidget> MyWidget;
    uint64 InstanceDataHash = 0;
    
    PROPERTY_BINDING_IMPLEMENTATION(FSlateColor, ColorAndOpacity);
};
