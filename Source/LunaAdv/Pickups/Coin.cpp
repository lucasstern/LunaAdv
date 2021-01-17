// Lucas Stern Games


#include "Coin.h"
#include "Components/ArrowComponent.h"

ACoin::ACoin()
{

}

void ACoin::SetActorsOverlapping(const TArray<AActor*>& ActorsArray)
{
	ActorsOverlapping = ActorsArray;
}

void ACoin::OnPickup()
{
	UE_LOG(LogTemp, Warning, TEXT("Coin %s is being pickedUp"), *this->GetName());
}
