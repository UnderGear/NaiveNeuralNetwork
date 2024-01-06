export module CostFunctions;

import std.core;

namespace CostFunctions
{
	template <typename T>
	struct SquaredDelta
	{
		[[nodiscard]] constexpr T operator()(const T& LHS, const T& RHS) const
		{
			auto Delta{ LHS - RHS };
			return Delta * Delta;
		}
	};

	// Sum of Squared Errors
	float SSE(std::span<const float> ExpectedValues, std::span<const float> ObservedValues)
	{
		//.5 * sum((expected - observed)^2)

		auto Result{ .5f * std::transform_reduce(ExpectedValues.begin(), ExpectedValues.end(), ObservedValues.begin(), 0.f, std::plus<>(), SquaredDelta<float>()) };

		return Result;
	}

	float SquaredError(float ExpectedValue, float ObservedValue)
	{
		return .5f * SquaredDelta<float>{}(ExpectedValue, ObservedValue);
	}

	float SquaredErrorPrime(float ExpectedValue, float ObservedValue)
	{
		return ObservedValue - ExpectedValue;
	}

	export
	{
		struct CostFamily
		{
			std::function<float(float, float)> Function;
			std::function<float(float, float)> DerivativeFunction;
		};

		const inline CostFamily SquaredErrorFamily
		{
			&SquaredError, &SquaredErrorPrime
		};
	}
}
