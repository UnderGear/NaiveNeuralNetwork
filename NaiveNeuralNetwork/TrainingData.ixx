export module TrainingData;

import std.core;

import Matrix;

export namespace TrainingData
{
	using TrainingInput = std::vector<std::pair<Matrix, Matrix>>;

	struct TrainingSet
	{
		TrainingInput Input;
		std::string_view Name;
	};

	TrainingSet OR
	{
		{
			{
				Matrix{ 2, 1, { 1.f, 1.f  } },
				Matrix{ 1, 1, { 1.f } }
			},
			{
				Matrix{ 2, 1, { 1.f, 0.f } },
				Matrix{ 1, 1, { 1.f } }
			},
			{
				Matrix{ 2, 1, { 0.f, 1.f } },
				Matrix{ 1, 1, { 1.f } }
			},
			{
				Matrix{ 2, 1, { 0.f, 0.f } },
				Matrix{ 1, 1, { 0.f } }
			},
		},
		"OR"
	};

	TrainingSet AND
	{
		{
			{
				Matrix{ 2, 1, { 1.f, 1.f  } },
				Matrix{ 1, 1, { 1.f } }
			},
			{
				Matrix{ 2, 1, { 1.f, 0.f } },
				Matrix{ 1, 1, { 0.f } }
			},
			{
				Matrix{ 2, 1, { 0.f, 1.f } },
				Matrix{ 1, 1, { 0.f } }
			},
			{
				Matrix{ 2, 1, { 0.f, 0.f } },
				Matrix{ 1, 1, { 0.f } }
			},
		},
		"AND"
	};

	TrainingSet XOR
	{
		{
			{
				Matrix{ 2, 1, { 1.f, 1.f  } },
				Matrix{ 1, 1, { 0.f } }
			},
			{
				Matrix{ 2, 1, { 1.f, 0.f } },
				Matrix{ 1, 1, { 1.f } }
			},
			{
				Matrix{ 2, 1, { 0.f, 1.f } },
				Matrix{ 1, 1, { 1.f } }
			},
			{
				Matrix{ 2, 1, { 0.f, 0.f } },
				Matrix{ 1, 1, { 0.f } }
			},
		},
		"XOR"
	};

	TrainingSet NAND
	{
		{
			{
				Matrix{ 2, 1, { 1.f, 1.f  } },
				Matrix{ 1, 1, { 0.f } }
			},
			{
				Matrix{ 2, 1, { 1.f, 0.f } },
				Matrix{ 1, 1, { 1.f } }
			},
			{
				Matrix{ 2, 1, { 0.f, 1.f } },
				Matrix{ 1, 1, { 1.f } }
			},
			{
				Matrix{ 2, 1, { 0.f, 0.f } },
				Matrix{ 1, 1, { 1.f } }
			},
		},
		"NAND"
	};

	TrainingSet NOR
	{
		{
			{
				Matrix{ 2, 1, { 1.f, 1.f  } },
				Matrix{ 1, 1, { 0.f } }
			},
			{
				Matrix{ 2, 1, { 1.f, 0.f } },
				Matrix{ 1, 1, { 0.f } }
			},
			{
				Matrix{ 2, 1, { 0.f, 1.f } },
				Matrix{ 1, 1, { 0.f } }
			},
			{
				Matrix{ 2, 1, { 0.f, 0.f } },
				Matrix{ 1, 1, { 1.f } }
			},
		},
		"NOR"
	};

	TrainingSet XNOR
	{
		{
			{
				Matrix{ 2, 1, { 1.f, 1.f  } },
				Matrix{ 1, 1, { 1.f } }
			},
			{
				Matrix{ 2, 1, { 1.f, 0.f } },
				Matrix{ 1, 1, { 0.f } }
			},
			{
				Matrix{ 2, 1, { 0.f, 1.f } },
				Matrix{ 1, 1, { 0.f } }
			},
			{
				Matrix{ 2, 1, { 0.f, 0.f } },
				Matrix{ 1, 1, { 1.f } }
			},
		},
		"XNOR"
	};
}
