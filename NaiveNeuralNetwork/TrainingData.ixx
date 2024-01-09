export module TrainingData;

import std.core;

import Matrix;

export namespace TrainingData
{
	using TrainingInput = std::vector<std::pair<Matrix, Matrix>>;

	struct TrainingSet
	{
		TrainingInput Input;
		std::string Name;
	};
}
