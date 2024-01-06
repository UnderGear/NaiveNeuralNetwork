export module Model;

import std.core;

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
	std::vector<Matrix> Parameters;

	Model(const Hyperparameters& Hypers)
	{
		Parameters.reserve(Hypers.Depth);
		for (auto i{ 0 }; i < Hypers.Depth; ++i)
		{
			// Note that i here is an index into NeuronsPerLayer, which includes the input.
			// Other indexing has the first hidden layer as 0 and excludes the input
			auto IncomingCount{ Hypers.NeuronsPerLayer[i] + Hypers.BiasCountPerLayer };
			auto NeuronCount{ Hypers.NeuronsPerLayer[i + 1] };

			std::vector<float> InputWeights(IncomingCount * NeuronCount);
			InitializeRandomWeights({ InputWeights.begin(), InputWeights.end() });
			Parameters.emplace_back(NeuronCount, IncomingCount, std::move(InputWeights));
		}
	}

	Matrix& operator [](std::size_t Index) { return Parameters[Index]; }
	const Matrix& operator [](std::size_t Index) const { return Parameters[Index]; }
};
