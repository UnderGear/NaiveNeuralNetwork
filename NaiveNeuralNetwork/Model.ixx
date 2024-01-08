export module Model;

import std.core;

import ActivationFunctions;
import CommandLineInput;
import CostFunctions;
import Hyperparameters;
import Matrix;

//TODO: read about different weight initialization schemes
// What's Glorot/Xavier initialization?
void InitializeRandomWeights(std::span<float> Weights)
{
	std::random_device RandomDevice;
	std::mt19937 RandomEngine{ RandomDevice() };
	std::uniform_real_distribution<float> Distribution{ 0.f, 1.f };

	std::ranges::generate(Weights, [&]() { return Distribution(RandomEngine); });
}

export struct Model
{
	static constexpr float Bias{ 1.f };
	static constexpr int BiasCountPerLayer{ 1 };

	Hyperparameters Hypers;
	std::vector<Matrix> Parameters;

	Model(Hyperparameters&& InHypers) : Hypers{ std::move(InHypers) }
	{
		Parameters.reserve(GetDepth());
		for (auto i{ 0 }; i < GetDepth(); ++i)
		{
			// Note that i here is an index into NeuronsPerLayer, which includes the input.
			// Other indexing has the first hidden layer as 0 and excludes the input
			auto IncomingCount{ Hypers.NeuronsPerLayer[i] + BiasCountPerLayer };
			auto NeuronCount{ Hypers.NeuronsPerLayer[i + 1] };

			std::vector<float> InputWeights(IncomingCount * NeuronCount);
			InitializeRandomWeights({ InputWeights.begin(), InputWeights.end() });
			Parameters.emplace_back(NeuronCount, IncomingCount, std::move(InputWeights));
		}
	}

	int GetDepth() const { return static_cast<int>(Hypers.NeuronsPerLayer.size()) - 1; } // Input layer doesn't count here
	int GetInputCount() const { return Hypers.NeuronsPerLayer.front(); }
	int GetOutputCount() const { return Hypers.NeuronsPerLayer.back(); }

	Matrix& operator [](std::size_t Index) { return Parameters[Index]; }
	const Matrix& operator [](std::size_t Index) const { return Parameters[Index]; }
};

export Model InitializeModel(const CommandLineInput::CommandLineParams& CommandLine)
{
	//TODO: see if we can load a model from the input (if specified)
// 	if (CommandLine.InputModelPath.has_value())
// 	{
// 		//TODO: load model from file provided. freak out if not provided
// 	}
// 	else
	{
		std::vector<ActivationFunctions::ActivationFamily> ActivationFamilies;
		std::ranges::transform(CommandLine.ActivationFamilies.begin(), CommandLine.ActivationFamilies.end(), std::back_inserter(ActivationFamilies), [](const std::string_view FamilyName)
			{
				return *ActivationFunctions::FamiliesByName.find(FamilyName)->second;
			});

		Hyperparameters Hypers
		{
			CommandLine.NeuronsPerLayer,
			std::move(ActivationFamilies),
			*(CostFunctions::FamiliesByName.find(*CommandLine.CostFamily)->second)
		};

		//TODO: validate hyperparameters

		return { std::move(Hypers) };
	}
}
