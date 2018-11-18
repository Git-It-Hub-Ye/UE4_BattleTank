// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PawnSpawnBox.generated.h"

class UBoxComponent;

UCLASS()
class UE4_BATTLETANK_API APawnSpawnBox : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly)
	UBoxComponent * SpawnVolume;

	/** Radius of search (Size of actor to spawn) */
	UPROPERTY(EditAnywhere, Category = "Config")
	float SearchRadius;

	/** Pawn which will be spawned */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<APawn> BP_ActorToSpawn;

	FVector TargetLocation;
	
public:	
	/** Sets default values for this actor's properties */
	APawnSpawnBox();

	/** Spawns AI pawns inside box, at random empty locations or return false */
	bool SpawnChosenActor();

protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

private:
	/** Generates random positions */
	bool PlaceSpawnActor();

	/** Returns true if location is empty */
	bool FindEmptyLocation(FVector & OutLocation);

	/** Checks if selected location is empty */
	bool CanSpawnAtLocation(FVector Location, float Radius);

	/** Spawns AI pawns inside box, at random empty locations */
	void PlaceActor(TSubclassOf<APawn> ToSpawn);
	
};
