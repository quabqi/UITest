#pragma once
#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"

#include "TestViewModel.generated.h"

// Blueprintable 必须写，默认的VM不支持蓝图
UCLASS(Blueprintable, DisplayName="Test ViewModel")
class UTestViewModel : public UMVVMViewModelBase
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
    float Scale = 1.0f;
    
private:
    void SetScale(float Value)
    {
        UE_MVVM_SET_PROPERTY_VALUE(Scale, Value);
    }
    float GetScale() const
    {
        return Scale;
    }

};

