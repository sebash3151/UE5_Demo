// Fill out your copyright notice in the Description page of Project Settings.


#include "AUE5DemotGameModeBase.h"
#include "Player_Character.h"

AAUE5DemotGameModeBase::AAUE5DemotGameModeBase()
{
	DefaultPawnClass = APlayer_Character::StaticClass();

	static ConstructorHelpers::FClassFinder<APlayer_Character> PlayerCharacterBP(TEXT("/Game/Blueprints/Player/Player_Character_BP"));
	if (PlayerCharacterBP.Class != NULL)
	{
		DefaultPawnClass = PlayerCharacterBP.Class;
	}
}
