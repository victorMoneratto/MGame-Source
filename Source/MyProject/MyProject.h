// Copyright 2018 Victor Moneratto. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

//////////////////////////////////////////////////////////////////////////
// Collision
#define COLLISION_WEAPON ECC_GameTraceChannel1

//////////////////////////////////////////////////////////////////////////
// Debugging
#define MDEBUG_LEVEL_LOG 1
#define MDEBUG_LEVEL_DRAW 2

FORCEINLINE FString GetRoleString(ENetRole Role) {
	return Role == ROLE_Authority ? TEXT("Server") : TEXT("Client");
}