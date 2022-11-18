#include "SInventoryWidget.h"

#include "Interfaces/ISlateRHIRendererModule.h"

SLATE_IMPLEMENT_WIDGET(SInventoryWidget)

void SInventoryWidget::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
    SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "Item", ItemAttribute,
        EInvalidateWidgetReason::Layout);
    SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "ColorAndOpacity", ColorAndOpacityAttribute,
        EInvalidateWidgetReason::Paint);
    SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(AttributeInitializer, "DesiredSizeOverride",
        DesiredSizeOverrideAttribute, EInvalidateWidgetReason::Layout);
}

SInventoryWidget::SInventoryWidget()
    : ItemAttribute(*this)
    , ColorAndOpacityAttribute(*this)
    , DesiredSizeOverrideAttribute(*this)
    , NumberOfColumns(0)
    , NumberOfRows(0)
    , DrawScale(1)
{
}

void SInventoryWidget::Construct(const FArguments& InArgs)
{
    ItemAttribute.Assign(*this, InArgs._Item);
    ColorAndOpacityAttribute.Assign(*this, InArgs._ColorAndOpacity);
    DesiredSizeOverrideAttribute.Assign(*this, InArgs._DesiredSizeOverride);
}

void SInventoryWidget::SetColorAndOpacity(TAttribute<FSlateColor> InColorAndOpacity)
{
    ColorAndOpacityAttribute.Assign(*this, MoveTemp(InColorAndOpacity));
}

void SInventoryWidget::SetColorAndOpacity(FLinearColor InColorAndOpacity)
{
    ColorAndOpacityAttribute.Set(*this, InColorAndOpacity);
}

void SInventoryWidget::SetItemBrush(TAttribute<const FSlateBrush*> InItemBrush)
{
    ItemAttribute.Assign(*this, MoveTemp(InItemBrush));
}

void SInventoryWidget::SetDesiredSizeOverride(TAttribute<TOptional<FVector2D>> InDesiredSizeOverride)
{
    DesiredSizeOverrideAttribute.Assign(*this, MoveTemp(InDesiredSizeOverride));
}

FVector2D SInventoryWidget::GetDesiredSizeOverride() const
{
    return ComputeDesiredSize(GetPrepassLayoutScaleMultiplier());
}

float SInventoryWidget::GetDrawScale() const
{
    return DrawScale;
}

void SInventoryWidget::SetNumberOfColumns(int32 InNumberOfColumns)
{
    if (NumberOfColumns != InNumberOfColumns)
    {
        NumberOfColumns = InNumberOfColumns;
    }
}

void SInventoryWidget::SetNumberOfRows(int32 InNumberOfRows)
{
    if (NumberOfRows != InNumberOfRows)
    {
        NumberOfRows = InNumberOfRows;
    }
}

static TArray<FSlateVertex> MyVertexData;
static TArray<SlateIndex> MyIndexData;

int32 SInventoryWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
    const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
    const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    const FSlateBrush* ImageBrush = ItemAttribute.Get();
    if (ImageBrush == nullptr)
    {
        return LayerId;
    }
    SInventoryWidget* MutableThis = const_cast<SInventoryWidget*>(this);
    const ESlateBrushDrawType::Type DrawType = ImageBrush->DrawAs;
    const FLinearColor FinalColorAndOpacity(
        InWidgetStyle.GetColorAndOpacityTint() * ColorAndOpacityAttribute.Get().GetColor(InWidgetStyle) * ImageBrush->
        GetTint(InWidgetStyle));
    const FColor PrimaryColor = FinalColorAndOpacity.ToFColor(true);
    const FPaintGeometry PaintGeometry = AllottedGeometry.ToPaintGeometry();
    const FSlateRenderTransform SlateRenderTransform = PaintGeometry.GetAccumulatedRenderTransform();
    
    const FVector2D LocalSize = GetDesiredSizeOverride() / FVector2D(NumberOfColumns, NumberOfRows);
    MutableThis->DrawScale = PaintGeometry.DrawScale;
    
    // Do pixel snapping
    FVector2D TopLeft(0, 0);
    FVector2D BotRight(LocalSize.X, LocalSize.Y);

    uint32 TextureWidth = 1;
    uint32 TextureHeight = 1;

    // Get the default start and end UV.  If the texture is atlased this value will be a subset of this
    FVector2D StartUV = FVector2D(0.0f, 0.0f);
    FVector2D EndUV = FVector2D(1.0f, 1.0f);
    FVector2D SizeUV = FVector2D(1.0f, 1.0f);

    const FSlateShaderResourceProxy* ResourceProxy = nullptr;
    MutableThis->ResourceHandle = ImageBrush->GetRenderingResource(LocalSize, DrawScale);
    if (ResourceHandle.IsValid())
    {
        ResourceProxy = ResourceHandle.GetResourceProxy();
    }
    if (ResourceProxy)
    {
        // The width and height of the texture (non-atlased size)
        TextureWidth = ResourceProxy->ActualSize.X != 0 ? ResourceProxy->ActualSize.X : 1;
        TextureHeight = ResourceProxy->ActualSize.Y != 0 ? ResourceProxy->ActualSize.Y : 1;

        const FBox2D& BrushUV = ImageBrush->GetUVRegion();;
        //In case brush has valid UV region - use it instead of proxy UV
        if (BrushUV.bIsValid)
        {
            SizeUV = BrushUV.GetSize();
            StartUV = BrushUV.Min;
            EndUV = StartUV + SizeUV;
        }
        else
        {
            SizeUV = FVector2D(ResourceProxy->SizeUV);
            StartUV = FVector2D(ResourceProxy->StartUV);
            EndUV = StartUV + FVector2D(ResourceProxy->SizeUV);
        }
    }

    const ESlateBrushTileType::Type TilingRule = ImageBrush->GetTiling();
    const bool bTileHorizontal = (TilingRule == ESlateBrushTileType::Both
        || TilingRule == ESlateBrushTileType::Horizontal);
    const bool bTileVertical = (TilingRule == ESlateBrushTileType::Both
        || TilingRule == ESlateBrushTileType::Vertical);

    const ESlateBrushMirrorType::Type MirroringRule = ImageBrush->GetMirroring();
    const bool bMirrorHorizontal = (MirroringRule == ESlateBrushMirrorType::Both
        || MirroringRule == ESlateBrushMirrorType::Horizontal);
    const bool bMirrorVertical = (MirroringRule == ESlateBrushMirrorType::Both
        || MirroringRule == ESlateBrushMirrorType::Vertical);

    ESlateBatchDrawFlag DrawFlags = ESlateBatchDrawFlag::None;
    DrawFlags |= (bTileHorizontal ? ESlateBatchDrawFlag::TileU : ESlateBatchDrawFlag::None)
        | (bTileVertical ? ESlateBatchDrawFlag::TileV : ESlateBatchDrawFlag::None);

    FColor SecondaryColor;
    EElementType ElementType = DrawType == ESlateBrushDrawType::Border ? EElementType::ET_Border : DrawType == ESlateBrushDrawType::RoundedBox ? EElementType::ET_RoundedBox : EElementType::ET_Box;
    if (ElementType == EElementType::ET_RoundedBox)
    {
        FLinearColor OutlineColor = ImageBrush->OutlineSettings.Color.GetSpecifiedColor();
        if (ImageBrush->OutlineSettings.bUseBrushTransparency)
        {
            OutlineColor.A = FinalColorAndOpacity.A;
        }
        SecondaryColor = OutlineColor.ToFColor(true);
    }

    float HorizontalTiling = bTileHorizontal ? LocalSize.X / TextureWidth : 1.0f;
    float VerticalTiling = bTileVertical ? LocalSize.Y / TextureHeight : 1.0f;

    const FVector2D Tiling(HorizontalTiling, VerticalTiling);

    // The start index of these vertices in the index buffer
    const FMargin& Margin = ImageBrush->GetMargin();

    const FVector2D TopRight = FVector2D(BotRight.X, TopLeft.Y);
    const FVector2D BotLeft = FVector2D(TopLeft.X, BotRight.Y);
    if (DrawType != ESlateBrushDrawType::Image && (Margin.Left != 0.0f || Margin.Top != 0.0f || Margin.Right != 0.0f ||
        Margin.Bottom != 0.0f))
    {
        // Create 9 quads for the box element based on the following diagram
        //     ___LeftMargin    ___RightMargin
        //    /                /
        //  +--+-------------+--+
        //  |  |c1           |c2| ___TopMargin
        //  +--o-------------o--+
        //  |  |             |  |
        //  |  |c3           |c4|
        //  +--o-------------o--+
        //  |  |             |  | ___BottomMargin
        //  +--+-------------+--+

        // Determine the texture coordinates for each quad
        // These are not scaled.
        float LeftMarginU = (Margin.Left > 0.0f) ? StartUV.X + Margin.Left * SizeUV.X : StartUV.X;
        float TopMarginV = (Margin.Top > 0.0f) ? StartUV.Y + Margin.Top * SizeUV.Y : StartUV.Y;
        float RightMarginU = (Margin.Right > 0.0f) ? EndUV.X - Margin.Right * SizeUV.X : EndUV.X;
        float BottomMarginV = (Margin.Bottom > 0.0f) ? EndUV.Y - Margin.Bottom * SizeUV.Y : EndUV.Y;

        if (bMirrorHorizontal || bMirrorVertical)
        {
            const FVector2D UVMin = StartUV;
            const FVector2D UVMax = EndUV;

            if (bMirrorHorizontal)
            {
                StartUV.X = UVMax.X - (StartUV.X - UVMin.X);
                EndUV.X = UVMax.X - (EndUV.X - UVMin.X);
                LeftMarginU = UVMax.X - (LeftMarginU - UVMin.X);
                RightMarginU = UVMax.X - (RightMarginU - UVMin.X);
            }
            if (bMirrorVertical)
            {
                StartUV.Y = UVMax.Y - (StartUV.Y - UVMin.Y);
                EndUV.Y = UVMax.Y - (EndUV.Y - UVMin.Y);
                TopMarginV = UVMax.Y - (TopMarginV - UVMin.Y);
                BottomMarginV = UVMax.Y - (BottomMarginV - UVMin.Y);
            }
        }

        // Determine the margins for each quad
        float LeftMarginX = TextureWidth * Margin.Left;
        float TopMarginY = TextureHeight * Margin.Top;
        float RightMarginX = LocalSize.X - TextureWidth * Margin.Right;
        float BottomMarginY = LocalSize.Y - TextureHeight * Margin.Bottom;

        // If the margins are overlapping the margins are too big or the button is too small
        // so clamp margins to half of the box size
        if (RightMarginX < LeftMarginX)
        {
            LeftMarginX = LocalSize.X / 2;
            RightMarginX = LeftMarginX;
        }

        if (BottomMarginY < TopMarginY)
        {
            TopMarginY = LocalSize.Y / 2;
            BottomMarginY = TopMarginY;
        }

        FVector2D Position = TopLeft;
        FVector2D EndPos = BotRight;

        MyVertexData.Reset(16);
        MyVertexData.AddUninitialized(16);
        MyVertexData[0] = FSlateVertex::Make<ESlateVertexRounding::Enabled>(SlateRenderTransform, FVector2f(Position.X, Position.Y),
            FVector2f(LocalSize), DrawScale, FVector4f(FVector2f(StartUV), FVector2f(Tiling)), PrimaryColor, SecondaryColor); //0
        MyVertexData[1] = FSlateVertex::Make<ESlateVertexRounding::Enabled>(SlateRenderTransform, FVector2f(Position.X, TopMarginY),
            FVector2f(LocalSize), DrawScale, FVector4f(FVector2f(StartUV.X, TopMarginV), FVector2f(Tiling)), PrimaryColor, SecondaryColor); //1
        MyVertexData[2] = FSlateVertex::Make<ESlateVertexRounding::Enabled>(SlateRenderTransform, FVector2f(LeftMarginX, Position.Y),
            FVector2f(LocalSize), DrawScale, FVector4f(FVector2f(LeftMarginU, StartUV.Y), FVector2f(Tiling)), PrimaryColor, SecondaryColor); //2
        MyVertexData[3] = FSlateVertex::Make<ESlateVertexRounding::Enabled>(SlateRenderTransform, FVector2f(LeftMarginX, TopMarginY),
            FVector2f(LocalSize), DrawScale, FVector4f(FVector2f(LeftMarginU, TopMarginV), FVector2f(Tiling)), PrimaryColor, SecondaryColor); //3
        MyVertexData[4] = FSlateVertex::Make<ESlateVertexRounding::Enabled>(SlateRenderTransform, FVector2f(RightMarginX, Position.Y),
            FVector2f(LocalSize), DrawScale, FVector4f(FVector2f(RightMarginU, StartUV.Y), FVector2f(Tiling)), PrimaryColor, SecondaryColor); //4
        MyVertexData[5] = FSlateVertex::Make<ESlateVertexRounding::Enabled>(SlateRenderTransform, FVector2f(RightMarginX, TopMarginY),
            FVector2f(LocalSize), DrawScale, FVector4f(FVector2f(RightMarginU, TopMarginV), FVector2f(Tiling)), PrimaryColor, SecondaryColor); //5
        MyVertexData[6] = FSlateVertex::Make<ESlateVertexRounding::Enabled>(SlateRenderTransform, FVector2f(EndPos.X, Position.Y),
            FVector2f(LocalSize), DrawScale, FVector4f(FVector2f(EndUV.X, StartUV.Y), FVector2f(Tiling)), PrimaryColor, SecondaryColor); //6
        MyVertexData[7] = FSlateVertex::Make<ESlateVertexRounding::Enabled>(SlateRenderTransform, FVector2f(EndPos.X, TopMarginY),
            FVector2f(LocalSize), DrawScale, FVector4f(FVector2f(EndUV.X, TopMarginV), FVector2f(Tiling)), PrimaryColor, SecondaryColor); //7

        MyVertexData[8] = FSlateVertex::Make<ESlateVertexRounding::Enabled>(SlateRenderTransform, FVector2f(Position.X, BottomMarginY),
            FVector2f(LocalSize), DrawScale, FVector4f(FVector2f(StartUV.X, BottomMarginV), FVector2f(Tiling)), PrimaryColor, SecondaryColor); //8
        MyVertexData[9] = FSlateVertex::Make<ESlateVertexRounding::Enabled>(SlateRenderTransform, FVector2f(LeftMarginX, BottomMarginY),
            FVector2f(LocalSize), DrawScale, FVector4f(FVector2f(LeftMarginU, BottomMarginV), FVector2f(Tiling)), PrimaryColor, SecondaryColor); //9
        MyVertexData[10] = FSlateVertex::Make<ESlateVertexRounding::Enabled>(SlateRenderTransform, FVector2f(RightMarginX, BottomMarginY),
            FVector2f(LocalSize), DrawScale, FVector4f(FVector2f(RightMarginU, BottomMarginV), FVector2f(Tiling)), PrimaryColor, SecondaryColor); //10
        MyVertexData[11] = FSlateVertex::Make<ESlateVertexRounding::Enabled>(SlateRenderTransform, FVector2f(EndPos.X, BottomMarginY),
            FVector2f(LocalSize), DrawScale, FVector4f(FVector2f(EndUV.X, BottomMarginV), FVector2f(Tiling)), PrimaryColor, SecondaryColor); //11
        MyVertexData[12] = FSlateVertex::Make<ESlateVertexRounding::Enabled>(SlateRenderTransform, FVector2f(Position.X, EndPos.Y),
            FVector2f(LocalSize), DrawScale, FVector4f(FVector2f(StartUV.X, EndUV.Y), FVector2f(Tiling)), PrimaryColor, SecondaryColor); //12
        MyVertexData[13] = FSlateVertex::Make<ESlateVertexRounding::Enabled>(SlateRenderTransform, FVector2f(LeftMarginX, EndPos.Y),
            FVector2f(LocalSize), DrawScale, FVector4f(FVector2f(LeftMarginU, EndUV.Y), FVector2f(Tiling)), PrimaryColor, SecondaryColor); //13
        MyVertexData[14] = FSlateVertex::Make<ESlateVertexRounding::Enabled>(SlateRenderTransform, FVector2f(RightMarginX, EndPos.Y),
            FVector2f(LocalSize), DrawScale, FVector4f(FVector2f(RightMarginU, EndUV.Y), FVector2f(Tiling)), PrimaryColor, SecondaryColor); //14
        MyVertexData[15] = FSlateVertex::Make<ESlateVertexRounding::Enabled>(SlateRenderTransform, FVector2f(EndPos.X, EndPos.Y),
            FVector2f(LocalSize), DrawScale, FVector4f(FVector2f(EndUV), FVector2f(Tiling)), PrimaryColor, SecondaryColor); //15

        MyIndexData.Reset(54);
        MyIndexData.AddUninitialized(54);
        // Top
        MyIndexData[0] = 0;
        MyIndexData[1] = 1;
        MyIndexData[2] = 2;
        MyIndexData[3] = 2;
        MyIndexData[4] = 1;
        MyIndexData[5] = 3;

        MyIndexData[6] = 2;
        MyIndexData[7] = 3;
        MyIndexData[8] = 4;
        MyIndexData[9] = 4;
        MyIndexData[10] = 3;
        MyIndexData[11] = 5;

        MyIndexData[12] = 4;
        MyIndexData[13] = 5;
        MyIndexData[14] = 6;
        MyIndexData[15] = 6;
        MyIndexData[16] = 5;
        MyIndexData[17] = 7;

        // Middle
        MyIndexData[18] = 1;
        MyIndexData[19] = 8;
        MyIndexData[20] = 3;
        MyIndexData[21] = 3;
        MyIndexData[22] = 8;
        MyIndexData[23] = 9;

        MyIndexData[24] = 3;
        MyIndexData[25] = 9;
        MyIndexData[26] = 5;
        MyIndexData[27] = 5;
        MyIndexData[28] = 9;
        MyIndexData[29] = 10;

        MyIndexData[30] = 5;
        MyIndexData[31] = 10;
        MyIndexData[32] = 7;
        MyIndexData[33] = 7;
        MyIndexData[34] = 10;
        MyIndexData[35] = 11;

        // Bottom
        MyIndexData[36] = 8;
        MyIndexData[37] = 12;
        MyIndexData[38] = 9;
        MyIndexData[39] = 9;
        MyIndexData[40] = 12;
        MyIndexData[41] = 13;

        MyIndexData[42] = 9;
        MyIndexData[43] = 13;
        MyIndexData[44] = 10;
        MyIndexData[45] = 10;
        MyIndexData[46] = 13;
        MyIndexData[47] = 14;

        MyIndexData[48] = 10;
        MyIndexData[49] = 14;
        MyIndexData[50] = 11;
        MyIndexData[51] = 11;
        MyIndexData[52] = 14;
        MyIndexData[53] = 15;
    }
    else
    {
        if (bMirrorHorizontal || bMirrorVertical)
        {
            const FVector2D UVMin = StartUV;
            const FVector2D UVMax = EndUV;

            if (bMirrorHorizontal)
            {
                StartUV.X = UVMax.X - (StartUV.X - UVMin.X);
                EndUV.X = UVMax.X - (EndUV.X - UVMin.X);
            }
            if (bMirrorVertical)
            {
                StartUV.Y = UVMax.Y - (StartUV.Y - UVMin.Y);
                EndUV.Y = UVMax.Y - (EndUV.Y - UVMin.Y);
            }
        }

        MyVertexData.Reset(4);
        MyVertexData.AddUninitialized(4);
        MyVertexData[0] = FSlateVertex::Make<ESlateVertexRounding::Enabled>(SlateRenderTransform, FVector2f(TopLeft),
            FVector2f(LocalSize), DrawScale, FVector4f(FVector2f(StartUV), FVector2f(Tiling)), PrimaryColor, SecondaryColor);
        MyVertexData[1] = FSlateVertex::Make<ESlateVertexRounding::Enabled>(SlateRenderTransform, FVector2f(TopRight),
            FVector2f(LocalSize), DrawScale, FVector4f(FVector2f(EndUV.X, StartUV.Y), FVector2f(Tiling)), PrimaryColor, SecondaryColor);
        MyVertexData[2] = FSlateVertex::Make<ESlateVertexRounding::Enabled>(SlateRenderTransform, FVector2f(BotLeft),
            FVector2f(LocalSize), DrawScale, FVector4f(FVector2f(StartUV.X, EndUV.Y), FVector2f(Tiling)), PrimaryColor, SecondaryColor);
        MyVertexData[3] = FSlateVertex::Make<ESlateVertexRounding::Enabled>(SlateRenderTransform, FVector2f(BotRight),
            FVector2f(LocalSize), DrawScale, FVector4f(FVector2f(EndUV), FVector2f(Tiling)), PrimaryColor, SecondaryColor);

        MyIndexData.Reset(6);
        MyIndexData.AddUninitialized(6);
        MyIndexData[0] = 0;
        MyIndexData[1] = 1;
        MyIndexData[2] = 2;
        MyIndexData[3] = 2;
        MyIndexData[4] = 1;
        MyIndexData[5] = 3;
    }

    if (ResourceHandle.IsValid())
    {
        if (PerInstanceBuffer.IsValid() && PerInstanceBuffer->GetNumInstances() > 1)
        {
            FSlateDrawElement::MakeCustomVerts(OutDrawElements, LayerId, ResourceHandle, MyVertexData, MyIndexData,
                PerInstanceBuffer.Get(), 0, PerInstanceBuffer->GetNumInstances());
        }
        else
        {
            FSlateDrawElement::MakeCustomVerts(OutDrawElements, LayerId, ResourceHandle, MyVertexData, MyIndexData, nullptr, 0, 0);
        }
    }
    return LayerId;
}

FVector2D SInventoryWidget::ComputeDesiredSize(float) const
{
    const TOptional<FVector2D>& CurrentSizeOverride = DesiredSizeOverrideAttribute.Get();
    return CurrentSizeOverride.IsSet() ? CurrentSizeOverride.GetValue() : FVector2D(600, 600);
}

static const FName SlateRHIModuleName("SlateRHIRenderer");

void SInventoryWidget::EnableInstancing(int32 InitialSize)
{
    if (!PerInstanceBuffer.IsValid())
    {
        auto& Module = FModuleManager::Get().GetModuleChecked<ISlateRHIRendererModule>(SlateRHIModuleName);
        PerInstanceBuffer = Module.CreateInstanceBuffer(InitialSize);
    }
}

void SInventoryWidget::UpdatePerInstanceBuffer(FSlateInstanceBufferData& Data)
{
    EnableInstancing(Data.Num());
    PerInstanceBuffer->Update(Data);
}
