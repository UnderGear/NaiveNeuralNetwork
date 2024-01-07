export module ModelEvaluation;

import std.core;

import Hyperparameters;
import Matrix;
import Model;
import Validation;

export namespace ModelEvaluation
{
	// Cached network data from a given evaluation
	struct EvaluationResult
	{
		// Keep track of data for use in backpropagation
		std::vector<Matrix> LinearCombinations;
		std::vector<Matrix> ActivationResults;
		std::vector<Matrix> LayerInputs; // Includes bias at index 0 on each layer

		EvaluationResult(const Hyperparameters& Hypers, const Matrix& Inputs)
		{
			LinearCombinations.resize(Hypers.GetDepth());
			ActivationResults.reserve(Hypers.GetDepth());
			LayerInputs.reserve(Hypers.GetDepth());

			for (auto i{ 0 }; i < Hypers.GetDepth(); ++i)
			{
				// Note that i here is an index into NeuronsPerLayer, which includes the input.
				// Other indexing has the first hidden layer as 0 and excludes the input
				auto IncomingCount{ Hypers.NeuronsPerLayer[i] + Model::BiasCountPerLayer };
				auto NeuronCount{ Hypers.NeuronsPerLayer[i + 1] };

				LayerInputs.emplace_back(IncomingCount, 1);
				ActivationResults.emplace_back(NeuronCount, 1);
			}

			// All inputs begin with a Bias value
			for (auto& LayerInput : LayerInputs)
			{
				LayerInput.Data.front() = Model::Bias;
			}

			// Set the input into our first layer input
			// Note that this skips index 0, which is reserved for Bias
			auto FirstNonBiasIter{ std::ranges::next(LayerInputs.front().Data.begin(), 1) };
			std::ranges::copy(Inputs.Data.begin(), Inputs.Data.end(), FirstNonBiasIter);
		}
	};

	EvaluationResult EvaluateModel(const Hyperparameters& Hypers, const Model& Parameters, const Matrix& Inputs)
	{
		EvaluationResult Result{ Hypers, Inputs };

		for (auto LayerIndex{ 0 }; LayerIndex < Hypers.GetDepth(); ++LayerIndex)
		{
			// Linear combination for all neuron inputs and weights in the layer
			auto& OutputLinearCombinations{ Result.LinearCombinations[LayerIndex] };
			OutputLinearCombinations = Result.LayerInputs[LayerIndex] * Parameters[LayerIndex];
			CheckValid(OutputLinearCombinations);

			auto& Results{ Result.ActivationResults[LayerIndex] };
			for (auto NeuronIndex{ 0 }; NeuronIndex < OutputLinearCombinations.Size(); ++NeuronIndex)
			{
				// Call activation function on each neuron after linear combination
				auto Result{ Hypers.ActivationFamilies[LayerIndex].Function(OutputLinearCombinations[NeuronIndex]) };
				Results[NeuronIndex] = Result;
				CheckValid(Result);
			}

			// copying activation function values over to the input for next layer (unless we're on the output layer)
			//TODO: can I just do this sort of thing in place instead?
			if (LayerIndex + 1 < Hypers.GetDepth())
			{
				auto FirstNonBiasIter{ std::ranges::next(Result.LayerInputs[LayerIndex + 1].Data.begin(), 1) };
				std::ranges::copy(Results.Data.begin(), Results.Data.end(), FirstNonBiasIter);
			}
		}

		return Result;
	}
}
