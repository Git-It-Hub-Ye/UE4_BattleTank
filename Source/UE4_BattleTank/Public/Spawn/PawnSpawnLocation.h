// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PawnSpawnLocation.generated.h"

class UBoxComponent;

/** Positions for each instance spawned */
USTRUCT()
struct FSpawnPosition {
	GENERATED_USTRUCT_BODY()

	FVector Location;
	float Rotation;
	float Scale;
};

UCLASS()
class UE4_BATTLETANK_API APawnSpawnLocation : public AActor
{
	GENERATED_BODY()

private:
	/** Box actors will spawn within */
	UPROPERTY(VisibleAnywhere)
	UBoxComponent * SpawnVolume;

	/** Radius of search (Size of actor to spawn) */
	UPROPERTY(EditAnywhere)
	float SearchRadius;

	FSpawnPosition SpawnPosition;

	/** Pawn which will be spawned */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<APawn> BPSpawnAIPawn;

public:	
	// Sets default values for this actor's properties
	APawnSpawnLocation();

	/** Spawns AI pawns inside box, at random empty locations or return false */
	bool TrySpawnActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	/** Generates random positions */
	void PlaceSpawnActor();

	/** Returns true if location is empty */
	bool FindEmptyLocation(FVector & OutLocation);

	/** Checks if selected location is empty */
	bool CanSpawnAtLocation(FVector Location, float Radius);

	/** Spawns AI pawns inside box, at random empty locations */
	void PlaceActor(TSubclassOf<APawn> ToSpawn);

};
