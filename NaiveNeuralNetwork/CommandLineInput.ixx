export module CommandLineInput;

import std.filesystem;

import ArgParsing;
import Enum;

export namespace CommandLineInput
{
	struct CommandLineParams
	{
		enum class RunFlags : std::uint8_t
		{
			Predict = 0b0001,
			Train = 0b0010,
		};

		// -t, -p, -tp/-pt
		RunFlags RunConfig;

		// Required if training
		// -td <path>
		// Path to training dataset
		std::optional<std::filesystem::path> TrainingDataPath;

		// Optional if training (will use default values if unspecified)
		//TODO: make these next 3 into optional vectors
		// -ld <int...>
		std::vector<int> NeuronsPerLayer;
		// -la <name...>
		std::vector<std::string_view> ActivationFamilies;
		// -c <name>
		std::optional<std::string_view> CostFamily;
		// -tr <float>
		std::optional<float> LearningRate;
		// -te <float>
		std::optional<float> ErrorTolerance;
		// -tm <int>
		std::optional<int> MaxIterations;
		// -mo <path>
		// If set and RunConfig includes Train, write the model to this file
		std::optional<std::filesystem::path> OutputModelPath;

		// Required if not training
		// -mi <path>
		std::optional<std::filesystem::path> InputModelPath;

		// Optional if not training

		//TODO: let's actually build this out and use it
		void Validate() const
		{
			// if training
			//TODO: check training path
	 		//TODO: check neurons per layer
	 		//TODO: check activation families
	 		//TODO: check cost family
			
	 		// else
	 		//TODO: warn about training only options being set?
			//TODO: check input model path set
		}
	};

	CommandLineParams ParseCommandLine(int argc, const char* argv[])
	{
		using namespace std::literals;
		ArgParsing::ArgParser<CommandLineParams> Parser;
		Parser.AddRule(
		{
			{ "-p"sv, CommandLineParams::RunFlags::Predict },
			{ "-t"sv, CommandLineParams::RunFlags::Train },
			{ "-pt"sv, Enum::AppendFlags(CommandLineParams::RunFlags::Predict, CommandLineParams::RunFlags::Train) },
			{ "-tp"sv, Enum::AppendFlags(CommandLineParams::RunFlags::Predict, CommandLineParams::RunFlags::Train) },
		}, &CommandLineParams::RunConfig);
		Parser.AddRule({ "-tr"sv }, &CommandLineParams::LearningRate);
		Parser.AddRule({ "-te"sv }, &CommandLineParams::ErrorTolerance);
		Parser.AddRule({ "-tm"sv }, &CommandLineParams::MaxIterations);
		Parser.AddRule({ "-td"sv }, &CommandLineParams::TrainingDataPath);
		Parser.AddRule({ "-mo"sv }, &CommandLineParams::OutputModelPath);
		Parser.AddRule({ "-mi"sv }, &CommandLineParams::InputModelPath);
		Parser.AddRule({ "-ld"sv }, &CommandLineParams::NeuronsPerLayer);
		Parser.AddRule({ "-la"sv }, &CommandLineParams::ActivationFamilies);
		Parser.AddRule({ "-c"sv }, &CommandLineParams::CostFamily);

		return Parser.Parse(argc, argv);
	}
}
