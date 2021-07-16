// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include <string>
#include <map>
#include <vector>
#include "MyActor2.generated.h"

USTRUCT(BlueprintType)
struct FVectors
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> Vector;
};

USTRUCT(BlueprintType)
struct FMolecules
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor *> Elements;
	TArray<int> Indices;
};

UCLASS()
class SPAWN_API AMyActor2 : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<AActor> CubeClass;

	UPROPERTY(EditAnywhere, Category = "Config")
	FString compound;

	UPROPERTY(EditAnywhere, Category = "Config")
	float distance;

	UPROPERTY(EditAnywhere, Category = "Config")
	float angle;

	AMyActor2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	AActor *SpawnObject(FVector Loc, FRotator Rot, int elementIndex);

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void RenderAdjacencyList();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void Spawning(int count, FVector v, float ang);

	UFUNCTION()
	int FindRoot(FString s1);

	UFUNCTION()
	void SpawnGraph(int previous, int current, TArray<bool> &visited, TArray<int> &molecule, TArray<AActor *> &elements);

	UFUNCTION()
	int GetAtomIndexByName(FString atom);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void NewMolecule();

	UFUNCTION(BlueprintCallable)
	TArray<FVectors> GetMoleculeGraph();

	UFUNCTION(BlueprintCallable)
	TArray<FString> GetMoleculeDecodeInfo();

	UFUNCTION(BlueprintCallable)
	int GetNOfGraph();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Event", meta = (DisplayName = "TestCall"))
	void BPEvent_TestCall(const int s);
};
