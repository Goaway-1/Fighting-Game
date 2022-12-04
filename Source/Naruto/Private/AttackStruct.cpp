// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackStruct.h"

// Sets default values
AAttackStruct::AAttackStruct()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AAttackStruct::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAttackStruct::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

