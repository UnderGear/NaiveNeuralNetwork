import std.core;

import CommandLineInput;
import Hyperparameters;
import Matrix;
import Model;
import ModelEvaluation;
import ModelTraining;
import TrainingData;

int main(int argc, const char* argv[])
{
	auto CommandLineParams{ CommandLineInput::ParseCommandLine(argc, argv) };
	//TODO: now actually use it.

	//TODO: based on the user params, either override its values with set optionals (training a new model)
	// or load a model file that should contain them
	Hyperparameters Hypers;
	TrainingParameters TrainingParams;

	//TODO: option for training a new model or loading an existing one from a file
	//TODO: save/load neural network data - dimensions, activation functions, weights, etc
	Model Parameters{ Hypers };

	//TODO: read training data from a file
	auto Errors{ ModelTraining::TrainModel(TrainingParams, Hypers, Parameters, TrainingData::XOR) };

	// Plug in user inputs, evaluate the model, print what it spits out the other end
	Matrix Inputs{ Hypers.GetInputCount(), 1 };
	while (true)
	{
		for (auto InputIndex{ 0 }; InputIndex < Hypers.GetInputCount(); ++InputIndex)
		{
			std::cout << "Input " << InputIndex << ": ";
			std::cin >> Inputs[InputIndex];
		}

		auto State{ ModelEvaluation::EvaluateModel(Hypers, Parameters, Inputs) };

		for (auto OutputIndex{ 0 }; OutputIndex < Hypers.GetOutputCount(); ++OutputIndex)
		{
			std::cout << "Output" << OutputIndex << ": " << State.ActivationResults.back()[OutputIndex] << std::endl;
		}
	}
}
