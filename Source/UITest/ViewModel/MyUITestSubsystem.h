// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMSubsystem.h"
#include "MyGlobalViewModel.h"
#include "UObject/Object.h"
#include "MyUITestSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class UITEST_API UMyUITestSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override
    {
        Super::Initialize(Collection);

        FMVVMViewModelContext Context1;
        Context1.ContextClass = UMyGlobalViewModel::StaticClass();
        Context1.ContextName = "MyGlobalViewModel1";
        
        FMVVMViewModelContext Context2;
        Context2.ContextClass = UMyGlobalViewModel::StaticClass();
        Context2.ContextName = "MyGlobalViewModel2";
        
        const auto ViewModel1 = NewObject<UMyGlobalViewModel>(this);
        const auto ViewModel2 = NewObject<UMyGlobalViewModel>(this);
        ViewModel2->FloatValue = 2.0f;
        ViewModel2->Int32Value = 2;
        ViewModel2->Int64Value = 2;
        ViewModel2->VectorValue = FVector(2.0, 2.0, 2.0);
        ViewModel2->Vector2DValue = FVector2D(2.0, 2.0);

        const auto GlobalViewModelCollection = GEngine->GetEngineSubsystem<UMVVMSubsystem>()->GetGlobalViewModelCollection();
        GlobalViewModelCollection->AddViewModelInstance(Context1, ViewModel1);
        GlobalViewModelCollection->AddViewModelInstance(Context2, ViewModel2);
    }
    
    virtual void Deinitialize() override
    {
        Super::Deinitialize();

        FMVVMViewModelContext Context1;
        Context1.ContextClass = UMyGlobalViewModel::StaticClass();
        Context1.ContextName = "MyGlobalViewModel1";

        FMVVMViewModelContext Context2;
        Context2.ContextClass = UMyGlobalViewModel::StaticClass();
        Context2.ContextName = "MyGlobalViewModel2";
        
        const auto GlobalViewModelCollection = GEngine->GetEngineSubsystem<UMVVMSubsystem>()->GetGlobalViewModelCollection();
        GlobalViewModelCollection->RemoveViewModel(Context1);
        GlobalViewModelCollection->RemoveViewModel(Context2);
    }
    
};
