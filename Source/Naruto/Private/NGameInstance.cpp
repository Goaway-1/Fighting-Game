#include "NGameInstance.h"
#include "AttackStruct.h"

UNGameInstance::UNGameInstance() {
    //FString CharacterDataPath = TEXT("/Game/DataTable/MontageData");
    //static ConstructorHelpers::FObjectFinder<UDataTable> DT_ABCHARACTER(*CharacterDataPath);

    //MontageTable = DT_ABCHARACTER.Object;
}
void UNGameInstance::Init() {
	Super::Init();

}
//FAttackMontageStruct* UNGameInstance::GetMontageData(int32 idx) {
//    return MontageTable->FindRow<FAttackMontageStruct>(*FString::FromInt(idx), TEXT(""));
//}