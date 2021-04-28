// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "MyActor2.generated.h"

UCLASS()
class SPAWN_API AMyActor2 : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties

	UPROPERTY(EditDefaultsOnly, Category="Spawning")
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
	void SpawnObject(FVector Loc, FRotator Rot, int elementIndex);

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void RenderAdjacencyList();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void Spawning(int count, FVector v, float ang);

	UFUNCTION()
	int FindRoot(FString s1);


public:	
	// Called every frame
	// virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Event", meta = (DisplayName = "TestCall")) 
	void BPEvent_TestCall(const int s);
};
