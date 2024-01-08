export module Hyperparameters;

import std.core;
import ActivationFunctions;
import CostFunctions;

export struct TrainingParameters
{
	// some people recommend starting with a large learning rate (like 1) and reducing it over training iterations
	// others suggest having it be a smaller value, but it may take a lot of iterations to converge
	float LearningRate{ .01f };
	float ErrorTolerance{ .00001f };
	int MaxIterations{ 1000 };
};

export struct Hyperparameters
{
	std::vector<int> NeuronsPerLayer{ 2, 5, 1 }; // input, hidden layers..., output

	// Activation function (and its derivative) to use on each layer after the input layer
	std::vector<ActivationFunctions::ActivationFamily> ActivationFamilies
	{
		ActivationFunctions::LeakyReLUFamily,
		ActivationFunctions::LeakyReLUFamily
	};

	CostFunctions::CostFamily CostFamily{ CostFunctions::SquaredErrorFamily };
};
