#include "InventoryWidget.h"

static FName InventoryColumnsName = "NumberOfColumns";
static FName InventoryRowsName = "NumberOfRows";
static FName InventoryWidthName = "Width";
static FName InventoryHeightName = "Height";

void UInventoryWidget::SetColorAndOpacity(FLinearColor InColorAndOpacity)
{
    ColorAndOpacity = InColorAndOpacity;
    if (MyWidget.IsValid())
    {
        MyWidget->SetColorAndOpacity(ColorAndOpacity);
    }
}

void UInventoryWidget::SetOpacity(float InOpacity)
{
    ColorAndOpacity.A = InOpacity;
    if (MyWidget.IsValid())
    {
        MyWidget->SetColorAndOpacity(ColorAndOpacity);
    }
}

void UInventoryWidget::SetDesiredSizeOverride(FVector2D DesiredSize)
{
    DesiredSizeOverride = DesiredSize;
    if (MyWidget.IsValid())
    {
        MyWidget->SetDesiredSizeOverride(DesiredSize);
        //UpdateMaterialParameterValue();
    }
}

void UInventoryWidget::SetNumberOfColumns(int32 InNumberOfColumns)
{
    NumberOfColumns = InNumberOfColumns;
    
    if (MyWidget.IsValid())
    {
        MyWidget->SetNumberOfColumns(FMath::Clamp(NumberOfColumns, 1, 25));
        //UpdateMaterialParameterValue();
    }
}

void UInventoryWidget::SetNumberOfRows(int32 InNumberOfRows)
{
    NumberOfRows = InNumberOfRows;
    if (MyWidget.IsValid())
    {
        MyWidget->SetNumberOfRows(FMath::Clamp(NumberOfRows, 1, 25));
        //UpdateMaterialParameterValue();
    }
}

const FSlateBrush* UInventoryWidget::ConvertImage(TAttribute<FSlateBrush> InImageAsset) const
{
    UInventoryWidget* MutableThis = const_cast<UInventoryWidget*>(this);
    MutableThis->Brush = InImageAsset.Get();
    return &Brush;
}

UMaterialInstanceDynamic* UInventoryWidget::GetDynamicMaterial()
{
    if (UMaterialInterface* Material = Cast<UMaterialInterface>(Brush.GetResourceObject()))
    {
        UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(Material);
        if (!DynamicMaterial)
        {
            DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
            Brush.SetResourceObject(DynamicMaterial);

            if (MyWidget.IsValid())
            {
                MyWidget->SetItemBrush(&Brush);
            }
        }
        return DynamicMaterial;
    }
    return nullptr;
}

TSharedRef<SWidget> UInventoryWidget::RebuildWidget()
{
    MyWidget = SNew(SInventoryWidget);
    return MyWidget.ToSharedRef();
}

void UInventoryWidget::ReleaseSlateResources(bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);
    MyWidget.Reset();
}

const FText UInventoryWidget::GetPaletteCategory()
{
    return NSLOCTEXT("quabqi ui", "Common", "InventoryWidget");
}

void UInventoryWidget::UpdateMaterialParameterValue()
{
    if (const auto Material = GetDynamicMaterial())
    {
        Material->SetScalarParameterValue(InventoryColumnsName, NumberOfColumns);
        Material->SetScalarParameterValue(InventoryRowsName, NumberOfRows);
        if (MyWidget.IsValid())
        {
            const FVector2D RealDrawSize = MyWidget->GetDesiredSizeOverride() * MyWidget->GetDrawScale();
            Material->SetScalarParameterValue(InventoryWidthName, RealDrawSize.X / NumberOfColumns);
            Material->SetScalarParameterValue(InventoryHeightName, RealDrawSize.Y / NumberOfRows);
        }
    }
}

void UInventoryWidget::SynchronizeProperties()
{
    Super::SynchronizeProperties();

    const TAttribute<FSlateColor> ColorAndOpacityBinding = PROPERTY_BINDING(FSlateColor, ColorAndOpacity);
    const TAttribute<const FSlateBrush*> ImageBinding = OPTIONAL_BINDING_CONVERT(FSlateBrush, Brush, const FSlateBrush*, ConvertImage);

    if (MyWidget.IsValid())
    {
        MyWidget->SetItemBrush(ImageBinding);
        MyWidget->SetColorAndOpacity(ColorAndOpacityBinding);

        MyWidget->SetNumberOfColumns(NumberOfColumns);
        MyWidget->SetNumberOfRows(NumberOfRows);
        MyWidget->SetDesiredSizeOverride(DesiredSizeOverride);
        //UpdateMaterialParameterValue();
        
        const uint64 Hash = CityHash64(reinterpret_cast<const char*>(InstanceData.GetData()), InstanceData.Num() * InstanceData.GetTypeSize());
        if (InstanceDataHash != Hash)
        {
            InstanceDataHash = Hash;
            FSlateInstanceBufferData Data;
            Data.AddUninitialized(InstanceData.Num());
            for (int32 Index = 0; Index < Data.Num(); ++Index)
            {
                Data[Index].X = Index;
                Data[Index].Y = InstanceData[Index].IconIndex;
                Data[Index].Z = InstanceData[Index].Count;
                Data[Index].W = InstanceData[Index].Quality;
            }
            MyWidget->UpdatePerInstanceBuffer(Data);
        }
    }
}
