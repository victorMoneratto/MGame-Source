// Copyright 2018 Victor Moneratto. All Rights Reserved.

#include "MPlayerController.h"
#include "MPlayerCameraManager.h"

AMPlayerController::AMPlayerController() {
	PlayerCameraManagerClass = AMPlayerCameraManager::StaticClass();
}
