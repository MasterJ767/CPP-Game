#include "Button.h"
#include "RenderObject.h"

#include "Vector4.h"

using namespace NCL::Maths;
using namespace NCL::CSC8503;

void Button::SwitchColour() {
	Vector4 colour = navGrid->GetDoorStatus() ? Vector4(0.25f, 0, 1, 1) : Vector4(1, 0, 0.25f, 1);

	renderObject->SetDefaultColour(colour);
	renderObject->SetColourToDefault();
}

void Button::OnCollisionBegin(GameObject* otherObject) {
	if (otherObject->GetCollisionLayer() == PLAYER) {
		navGrid->SwitchDoors();
		game->SwitchDoors();
		SwitchColour();
	}
}