// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SpawnPoint.generated.h"

/**
* Spawns actor at this location
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE4_BATTLETANK_API USpawnPoint : public USceneComponent
{
	GENERATED_BODY()

private:
	/** Actor to be spawned */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<AActor> ActorSpawnBP;

	UPROPERTY()
	AActor * SpawnedActor;

public:
	/** Sets default values and component */
	USpawnPoint();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	AActor * GetSpawnedActor() const { return SpawnedActor; }

protected:
	virtual void BeginPlay() override;
	
};
