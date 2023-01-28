#pragma once
namespace NCL::CSC8503 {
	enum Layer {
		DEFAULT = 0,
		ITEM = 1,
		PLAYER = 2,
		ENVIRONMENT = 4,
		HUMAN = 8,
		GOOSE = 16,
		ITEMDECAY = 32,
		BUTTON = 64,
		AXLE = 128
	};
}