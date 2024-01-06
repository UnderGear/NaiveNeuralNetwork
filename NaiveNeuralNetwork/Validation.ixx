export module Validation;

import std.core;

import Matrix;

export
{
	void CheckValid(float Value)
	{
		if (std::isnan(Value))
		{
			throw std::domain_error{ "Value is NaN" };
		}
		else if (std::isinf(Value))
		{
			throw std::domain_error{ "Value is inf" };
		}
	}

	void CheckValid(std::span<const float> Values)
	{
		for (auto Value : Values)
			CheckValid(Value);
	}

	void CheckValid(const Matrix& M)
	{
		CheckValid({ M.Data.begin(), M.Data.end() });
	}
}
