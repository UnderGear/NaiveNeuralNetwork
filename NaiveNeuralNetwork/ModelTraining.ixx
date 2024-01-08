export module ModelTraining;

import std.core;

import ActivationFunctions;
import CommandLineInput;
import CostFunctions;
import Hyperparameters;
import Matrix;
import Model;
import ModelEvaluation;
import TrainingData;
import Validation;

namespace ModelTraining
{
	export TrainingParameters GetTrainingParameters(const CommandLineInput::CommandLineParams& CommandLine)
	{
		TrainingParameters Params;
		if (CommandLine.LearningRate.has_value())
		{
			Params.LearningRate = *CommandLine.LearningRate;
		}
		if (CommandLine.ErrorTolerance.has_value())
		{
			Params.ErrorTolerance = *CommandLine.ErrorTolerance;
		}
		if (CommandLine.MaxIterations.has_value())
		{
			Params.MaxIterations = *CommandLine.MaxIterations;
		}
		return Params;
	}

	std::vector<float> Train(
		const TrainingParameters& TrainingParams,
		Model& Parameters,
		const Matrix& Inputs,
		const Matrix& ExpectedOutputs)
	{
		auto State{ ModelEvaluation::EvaluateModel(Parameters, Inputs) };

		// Calculate error on each output node's activation value
		std::vector<float> Errors;
		Errors.resize(ExpectedOutputs.Size());
		for (auto i{ 0 }; i < Errors.size(); ++i)
		{
			// Apply the cost function to our activation results to see how far off we are
			Errors[i] = Parameters.Hypers.CostFamily.Function(ExpectedOutputs[i], State.ActivationResults.back()[i]);

			// 		std::cout << "Output: " << State.ActivationResults.back()[i] << std::endl;
			// 		std::cout << "Expected: " << ExpectedOutputs[i] << std::endl;
			//		std::cout << "Error: " << Errors[i] << std::endl;
		}

		std::vector<Matrix> NeuronErrors;
		NeuronErrors.reserve(Parameters.GetDepth());
		for (auto i{ 0 }; i < Parameters.GetDepth(); ++i)
		{
			auto& Comparable{ State.ActivationResults[i] };
			NeuronErrors.emplace_back(Comparable.Width, Comparable.Height);
		}

		// Back propagation
		for (auto LayerIndex{ Parameters.GetDepth() - 1 }; LayerIndex >= 0; --LayerIndex)
		{
			auto& LayerActivations{ State.ActivationResults[LayerIndex] };
			auto& LayerLinearCombinations{ State.LinearCombinations[LayerIndex] };
			auto& LayerInputs{ State.LayerInputs[LayerIndex] };
			auto& LayerParams{ Parameters[LayerIndex] };
			auto& ActivationFamily{ Parameters.Hypers.ActivationFamilies[LayerIndex] };
			auto IsOutputLayer{ LayerIndex == (Parameters.GetDepth() - 1) };

			for (auto NeuronIndex{ 0 }; NeuronIndex < LayerActivations.Size(); ++NeuronIndex)
			{
				auto ActivationError{ 0.f };

				if (IsOutputLayer)
				{
					// Activation error attribution for output neurons is the derivative of our cost function
					ActivationError = Parameters.Hypers.CostFamily.DerivativeFunction(ExpectedOutputs[NeuronIndex], LayerActivations[NeuronIndex]);

					CheckValid(ActivationError);
				}
				else
				{
					// but for hidden layer neurons it's the linear combination of the weights and neuron errors on the next layer
					auto& NextLayerParams{ Parameters[LayerIndex + 1] };
					auto& NextLayerErrors{ NeuronErrors[LayerIndex + 1] };

					//TODO: pull it out of the for loop and just do a single matrix multiply for the whole layer?
					for (auto i{ 0 }; i < NextLayerParams.Height; ++i)
					{
						for (auto j{ 0 }; j < NextLayerErrors.Width; ++j)
						{
							ActivationError += NextLayerErrors[j] * NextLayerParams[NextLayerParams.GetIndex(i, j)];
							CheckValid(ActivationError);
						}
					}
				}

				auto LinearCombinationError{ ActivationFamily.DerivativeFunction(LayerLinearCombinations[NeuronIndex]) };

				// save this out into a vector of matrices. we'll need it in further passes.
				auto NeuronError{ ActivationError * LinearCombinationError };
				CheckValid(NeuronError);

				NeuronErrors[LayerIndex][NeuronIndex] = NeuronError;

				// now loop over the inputs going to this neuron and adjust our weights per input
				for (auto InputIndex{ 0 }; InputIndex < LayerInputs.Size(); ++InputIndex)
				{
					// dE/dActivation = CostFunctions::SquaredErrorPrime(Activation) (or whichever function we specify)
					// dActivation/dLinearCombination = ActivationFunctions::ReLUPrime(LinearCombination) (or whichever function we specify)
					// dLinearCombination/dWi = Input (I think that's just the input passed through the linear combination without the weight)

					// dE/Wi = dE/dActivation * dActivation/dLinearCombination * dLinearCombination/dWi

					// Now to update a weight going to the target neuron
					// Wi -= Parameters.Hypers.LearningRate * dE/Wi

					auto WeightIndex{ LayerParams.GetIndex(InputIndex, NeuronIndex) };
					auto& Weight{ LayerParams[WeightIndex] };

					// Adjust our weight with gradient descent
					Weight -= TrainingParams.LearningRate * NeuronError * LayerInputs[InputIndex];

					CheckValid(Weight);
				}
			}
		}

		return Errors;
	}

	struct TrainingResults
	{
		std::vector<std::vector<std::vector<float>>> ErrorsPerIteration;
		bool Success{ false };
	};

	//TODO: maybe it's time to make this into an actual struct, like TrainingResults...
	export TrainingResults TrainModel(
		const TrainingParameters& TrainingParams,
		Model& Parameters,
		const TrainingData::TrainingSet& TrainingSet)
	{
		std::cout << "Training starting with set " << TrainingSet.Name << std::endl;

		std::vector<std::vector<std::vector<float>>> ErrorsPerIteration(TrainingParams.MaxIterations);
		bool Success{ false };

		// Do up to MaxIterations of training across our training data
		for (auto Iterations{ 0 }; Iterations < TrainingParams.MaxIterations; ++Iterations)
		{
			auto& ErrorsThisIteration{ ErrorsPerIteration[Iterations] };
			ErrorsThisIteration.reserve(TrainingSet.Input.size());

			// So I could maybe pull this AllPassed calculation out to another algorithm, but the actual call to Train would be harder to see
			auto AllPassed{ true };
			for (auto TrialIndex{ 0 }; TrialIndex < TrainingSet.Input.size(); ++TrialIndex)
			{
				const auto& [Inputs, ExpectedOutputs] { TrainingSet.Input[TrialIndex] };

				// Actually call Train on the model
				auto TrainingResult{ Train(TrainingParams, Parameters, Inputs, ExpectedOutputs) };

				ErrorsThisIteration.push_back(std::move(TrainingResult));

				// Keep track of our error vs our tolerance. If we're within bounds for all entries in the training set, we're done.
				AllPassed &= std::ranges::none_of(ErrorsThisIteration[TrialIndex], [&](float Error)
				{
					return std::abs(Error) > TrainingParams.ErrorTolerance;
				});
			}

			if (AllPassed)
			{
				Success = true;
				std::cout << "Within error threshold - ";
				break;
			}
		}

		std::cout << "Training finished" << std::endl;
		return { std::move(ErrorsPerIteration), Success };
	}
}
