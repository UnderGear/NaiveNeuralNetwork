export module Matrix;

import std.core;

// I'd normally template by dimensions but this needs to be dynamic for various sizes of matrices defined at runtime
export struct Matrix
{
	//TODO: don't let people mess with the invariants. keep the member variables private and provide limited access to them.

	int Width{ 0 };
	int Height{ 0 };

	std::vector<float> Data;

	constexpr Matrix() = default;

	constexpr Matrix(int InWidth, int InHeight) : Width(InWidth), Height(InHeight)
	{
		Data.resize(Size());
	}

	constexpr Matrix(int InWidth, int InHeight, std::vector<float>&& InData)
		: Width(InWidth), Height(InHeight), Data(std::move(InData))
	{}

	int Size() const { return Width * Height; }

	int GetIndex(int Row, int Column) const
	{
		if (Row >= Height)
		{
			throw std::invalid_argument{ "Row >= Height" };
		}

		if (Column >= Width)
		{
			throw std::invalid_argument{ "Column >= Width" };
		}

		return Row * Width + Column % Width;
	}

	Matrix operator *(Matrix Other) const
	{
		if (Width != Other.Height)
		{
			throw std::invalid_argument{ "Invalid dimensions for matrix multiplication" };
		}

		Matrix Result
		{
			Other.Width,
			Height
		};

		//TODO: this can certainly be optimized
		for (int i{ 0 }; i < Height; ++i)
		{
			for (int j{ 0 }; j < Other.Width; ++j)
			{
				for (int k{ 0 }; k < Width; ++k)
				{
					Result.Data[Result.GetIndex(i, j)] += Data[GetIndex(i, k)] * Other.Data[Other.GetIndex(k, j)];
				}
			}
		}

		return Result;
	}

	float& operator [](std::size_t Index) { return Data[Index]; }
	const float& operator [](std::size_t Index) const { return Data[Index]; }
};
