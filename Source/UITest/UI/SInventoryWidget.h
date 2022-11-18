#pragma once

#include "CoreMinimal.h"
#include "Rendering/RenderingCommon.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SLeafWidget.h"

class SInventoryWidget : public SLeafWidget
{
    SLATE_DECLARE_WIDGET(SInventoryWidget, SLeafWidget)
public:
    SLATE_BEGIN_ARGS(SInventoryWidget)
            : _Item(FCoreStyle::Get().GetDefaultBrush())
            , _ColorAndOpacity(FLinearColor::White)
        {
        }

        /** Image resource */
        SLATE_ATTRIBUTE(const FSlateBrush*, Item)

        /** Color and opacity */
        SLATE_ATTRIBUTE(FSlateColor, ColorAndOpacity)

        /** When specified, ignore the brushes size and report the DesiredSizeOverride as the desired image size. */
        SLATE_ATTRIBUTE(TOptional<FVector2D>, DesiredSizeOverride)
    SLATE_END_ARGS()

    /** Constructor */
    SInventoryWidget();

    void Construct(const FArguments& InArgs);

    /** Set the ColorAndOpacity attribute */
    void SetColorAndOpacity(TAttribute<FSlateColor> InColorAndOpacity);

    /** See the ColorAndOpacity attribute */
    void SetColorAndOpacity(FLinearColor InColorAndOpacity);

    /** Set the Image attribute */
    void SetItemBrush(TAttribute<const FSlateBrush*> InItemBrush);

    /** Set SizeOverride attribute */
    void SetDesiredSizeOverride(TAttribute<TOptional<FVector2D>> InDesiredSizeOverride);

    FVector2D GetDesiredSizeOverride() const;
    float GetDrawScale() const;
    
    /** Sets how many columns there are */
    void SetNumberOfColumns(int32 InNumberOfColumns);

    /** Sets how many rows there are */
    void SetNumberOfRows(int32 InNumberOfRows);

    void EnableInstancing(int32 InitialSize);
    void UpdatePerInstanceBuffer(FSlateInstanceBufferData& Data);
private:
    // BEGIN SLeafWidget interface
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
        FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
        bool bParentEnabled) const override;
    virtual FVector2D ComputeDesiredSize(float) const override;
    // END SLeafWidget interface

    /** The slate brush to draw for the ImageAttribute that we can invalidate. */
    TSlateAttribute<const FSlateBrush*> ItemAttribute;

    /** Color and opacity scale for this ImageAttribute */
    TSlateAttribute<FSlateColor> ColorAndOpacityAttribute;

    /** When specified, ignore the content's desired size and report the.HeightOverride as the Box's desired height. */
    TSlateAttribute<TOptional<FVector2D>> DesiredSizeOverrideAttribute;

    TSharedPtr<ISlateUpdatableInstanceBuffer> PerInstanceBuffer;

    /** How many pieces there are */
    int32 NumberOfColumns;

    /** How many pieces there are */
    int32 NumberOfRows;

    float DrawScale;
    
    FSlateResourceHandle ResourceHandle;
};
