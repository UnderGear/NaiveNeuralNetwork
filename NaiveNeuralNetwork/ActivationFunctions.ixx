export module ActivationFunctions;

import std.core;

namespace ActivationFunctions
{
	// ReLU, Sigmoid, and TanH are commonly used for hidden layers
	// Linear, Sigmoid, and Softmax are commonly used for output layers

	// Heaviside step function for classic Perceptron activation
	// Not used as much anymore because backpropagation requires a differentiable activation function
	float HeavisideStep(float Input)
	{
		return Input > 0.f ? 1.f : 0.f;
	}

	// AKA Logistic
	// Limits output between 0 and 1
	float Sigmoid(float Input)
	{
		return 1.f / (1.f + std::exp(-Input));
	}
	float SigmoidPrime(float Input)
	{
		return Sigmoid(Input) * (1 - Sigmoid(Input));
	}

	// Rectified Linear Unit, seems to be a very popular activation function right now
	// Apparently these have good qualities when doing back propagation
	float ReLU(float Input)
	{
		return std::max(0.f, Input);
	}
	float ReLUPrime(float Input)
	{
		return Input >= 0.f ? 1.f : 0.f;
	}

	float LeakyReLU(float Input)
	{
		return std::max(0.01f * Input, Input);
	}
	float LeakyReLUPrime(float Input)
	{
		return Input > 0.f ? 1.f : 0.01f;
	}

	// Outputs -1 to 1
	float TanH(float Input)
	{
		return std::tanh(Input);
	}
	float TanHPrime(float Input)
	{
		return 1.f - (TanH(Input) * TanH(Input));
	}

	float SoftPlus(float Input)
	{
		return std::log(1.f + std::exp(Input));
	}
	float SoftPlusPrime(float Input)
	{
		return Sigmoid(Input);
	}

	// Sigmoid Linear Unit
	float Swish(float Input)
	{
		return Input * Sigmoid(Input);
	}
	float SwishPrime(float Input)
	{
		return Input * SigmoidPrime(Input) + Sigmoid(Input);
	}

	float Identity(float Input)
	{
		return Input;
	}
	float IdentityPrime(float Input)
	{
		return 1.f;
	}

	export
	{
		struct ActivationFamily
		{
			std::function<float(float)> Function;
			std::function<float(float)> DerivativeFunction;
		};

		inline const ActivationFamily SigmoidFamily
		{
			&Sigmoid, &SigmoidPrime
		};

		inline const ActivationFamily ReLUFamily
		{
			&ReLU, &ReLUPrime
		};

		inline const ActivationFamily LeakyReLUFamily
		{
			&LeakyReLU, &LeakyReLUPrime
		};

		inline const ActivationFamily TanHFamily
		{
			&TanH, &TanHPrime
		};

		inline const ActivationFamily SoftPlusFamily
		{
			&SoftPlus, &SoftPlusPrime
		};

		inline const ActivationFamily SwishFamily
		{
			&Swish, &SwishPrime
		};

		inline const ActivationFamily IdentityFamily
		{
			&Identity, &IdentityPrime
		};
	}
}
