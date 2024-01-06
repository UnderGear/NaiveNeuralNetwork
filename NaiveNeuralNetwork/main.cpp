import std.core;

import ActivationFunctions;
import CostFunctions;
import Hyperparameters;
import Matrix;
import Model;
import ModelEvaluation;
import ModelTraining;
import TrainingData;

int main()
{
	//TODO: read hyperparameters from a file or command line args
	Hyperparameters Hypers;

	//TODO: option for training a new model or loading an existing one from a file
	//TODO: save/load neural network data - dimensions, activation functions, weights, etc
	Model Parameters{ Hypers };

	//TODO: read training data from a file
	auto Errors{ ModelTraining::TrainModel(Hypers, Parameters, TrainingData::XOR) };

	// Plug in user inputs, evaluate the model, print what it spits out the other end
	Matrix Inputs{ Hypers.InputCount, 1 };
	while (true)
	{
		for (auto InputIndex{ 0 }; InputIndex < Hypers.InputCount; ++InputIndex)
		{
			std::cout << "Input " << InputIndex << ": ";
			std::cin >> Inputs[InputIndex];
		}

		auto State{ ModelEvaluation::EvaluateModel(Hypers, Parameters, Inputs) };

		for (auto OutputIndex{ 0 }; OutputIndex < Hypers.OutputCount; ++OutputIndex)
		{
			std::cout << "Output" << OutputIndex << ": " << State.ActivationResults.back()[OutputIndex] << std::endl;
		}
	}
}
