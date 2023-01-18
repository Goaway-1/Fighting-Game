#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "PlayerDataTable.generated.h"

USTRUCT(BlueprintType)
struct FPlayerTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelUp")
	int32 ExpToNextLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelUp")
	int32 TotalExp;
};

UCLASS()
class NARUTO_API APlayerDataTable : public AActor
{
	GENERATED_BODY()

};
