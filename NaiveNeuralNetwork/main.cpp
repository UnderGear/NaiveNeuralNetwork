import std.core;

import CommandLineInput;
import Enum;
import Matrix;
import Model;
import ModelEvaluation;
import ModelTraining;
import TrainingData;

int main(int argc, const char* argv[])
{
	auto CommandLineParams{ CommandLineInput::ParseCommandLine(argc, argv) };

	//TODO: save/load neural network data - dimensions, activation functions, weights, etc
	Model Parameters{ InitializeModel(CommandLineParams) };

	if (Enum::ContainsFlag(CommandLineParams.RunConfig, CommandLineInput::CommandLineParams::RunFlags::Train))
	{
		auto TrainingParams{ ModelTraining::GetTrainingParameters(CommandLineParams) };
		
		//TODO: read training data from a file
		auto TrainingResults{ ModelTraining::TrainModel(TrainingParams, Parameters, TrainingData::XOR) };
		if (TrainingResults.Success == false)
		{
			std::cout << "training failed" << std::endl;
			throw std::exception{ "Training failed" };
		}

		//TODO: if there's an out model file, write to it.
	}

	//TODO: make a separate predict vs interactive mode? predict can handle command line inputs and interactive can be this?
	if (Enum::ContainsFlag(CommandLineParams.RunConfig, CommandLineInput::CommandLineParams::RunFlags::Predict))
	{
		// Plug in user inputs, evaluate the model, print what it spits out the other end
		Matrix Inputs{ Parameters.GetInputCount(), 1 };
		while (true)
		{
			for (auto InputIndex{ 0 }; InputIndex < Parameters.GetInputCount(); ++InputIndex)
			{
				std::cout << "Input " << InputIndex << ": ";
				std::cin >> Inputs[InputIndex];
			}

			auto State{ ModelEvaluation::EvaluateModel(Parameters, Inputs) };

			for (auto OutputIndex{ 0 }; OutputIndex < Parameters.GetOutputCount(); ++OutputIndex)
			{
				std::cout << "Output" << OutputIndex << ": " << State.ActivationResults.back()[OutputIndex] << std::endl;
			}
		}
	}
}
