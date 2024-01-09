export module ArgParsing;

import std.core;
import std.filesystem;

namespace ArgParsing
{
	template <typename T, template<typename...> class Ref>
	struct IsSpecialization : std::false_type {};

	template <template<typename...> class T, typename... ArgTs>
	struct IsSpecialization<T<ArgTs...>, T> : std::true_type {};

	template <typename TargetT = void>
	TargetT ConvertArg(std::string_view Source)
	{
		return TargetT{};
	}

	//TODO: try to eliminate the std::optional from the front of these ConvertArg specializations
	template <>
	float ConvertArg(std::string_view Source)
	{
		float ConvertedValue;
		auto Result{ std::from_chars(Source.data(), Source.data() + Source.size(), ConvertedValue) };
		if (Result.ec == std::errc::invalid_argument)
		{
			throw std::invalid_argument{ "invalid float param" };
		}
		return ConvertedValue;
	}

	template <>
	int ConvertArg(std::string_view Source)
	{
		int ConvertedValue;
		auto Result{ std::from_chars(Source.data(), Source.data() + Source.size(), ConvertedValue) };
		if (Result.ec == std::errc::invalid_argument)
		{
			throw std::invalid_argument{ "invalid float param" };
		}
		return ConvertedValue;
	}

	template <>
	std::string_view ConvertArg(std::string_view Source)
	{
		return Source;
	}

	template <>
	std::filesystem::path ConvertArg(std::string_view Source)
	{
		return { Source };
	}

	//TODO: it'd be nice to have this be able to call into the plural version, but I think the design would have to change
	template <template<typename> class T, typename ArgT>
	T<ArgT> ConvertArg(std::string_view Source)
	{
		return { ConvertArg<ArgT>(Source) };
	}

	template <template<typename> class T, typename ArgT>
	T<ArgT> ConvertArgs(std::string_view Source)
	{
		using namespace std::literals;

		T<ArgT> Result;
		for (auto ArgumentString : std::views::split(Source, ","sv))
		{
			Result.emplace_back(ConvertArg<ArgT>(std::string_view{ ArgumentString }));
		}

		return Result;
	}

	template <typename TargetT>
	struct ArgParamBase
	{
		virtual void Activate(TargetT& Target, std::string_view Arg, std::optional<std::string_view> Param) = 0;
		virtual bool CausesActivation(std::string_view Arg) const = 0;

		void Evaluate(TargetT& Target, std::string_view Arg, std::optional<std::string_view> Param)
		{
			if (CausesActivation(Arg))
				Activate(Target, Arg, Param);
		}
	};

	template <typename TargetT, typename ParamT>
	struct ArgRuleParam : public ArgParamBase<TargetT>
	{
		std::vector<std::string_view> Activations;
		ParamT TargetT::* TargetMember;

		ArgRuleParam(std::vector<std::string_view>&& InActivations, ParamT TargetT::* InTargetMember)
			: Activations{ InActivations }, TargetMember{ InTargetMember } {}

		virtual bool CausesActivation(std::string_view Arg) const override
		{
			return std::ranges::find(Activations, Arg) != Activations.end();
		}

		virtual void Activate(TargetT& Target, std::string_view Arg, std::optional<std::string_view> Param) override
		{
			if (Param == std::nullopt)
				throw std::invalid_argument{ "missing parameter" };

			Target.*TargetMember = ConvertArg<ParamT>(*Param);
		}
	};

	template <typename TargetT, template<typename> class ParamT, typename ArgT>
	struct ArgRuleParamContainer : public ArgParamBase<TargetT>
	{
		std::vector<std::string_view> Activations;
		ParamT<ArgT> TargetT::* TargetMember;

		ArgRuleParamContainer(std::vector<std::string_view>&& InActivations, ParamT<ArgT> TargetT::* InTargetMember)
			: Activations{ InActivations }, TargetMember{ InTargetMember } {}

		virtual bool CausesActivation(std::string_view Arg) const override
		{
			return std::ranges::find(Activations, Arg) != Activations.end();
		}

		virtual void Activate(TargetT& Target, std::string_view Arg, std::optional<std::string_view> Param) override
		{
			if (Param == std::nullopt)
				throw std::invalid_argument{ "missing parameter" };

			//TODO: I don't immediately see a better way of making this call
			// determine if the param should be parsed into a single entry in the target of if it should be split out.
			if constexpr (IsSpecialization<ParamT<ArgT>, std::optional>::value)
				Target.*TargetMember = ConvertArg<ParamT, ArgT>(*Param);
			else
				Target.*TargetMember = ConvertArgs<ParamT, ArgT>(*Param);
		}
	};

	template <typename TargetT, typename BackingT>
	struct ArgRuleFlags : public ArgParamBase<TargetT>
	{
		std::unordered_map<std::string_view, BackingT> Activations;
		BackingT TargetT::* TargetMember;

		ArgRuleFlags(std::unordered_map<std::string_view, BackingT>&& InActivations, BackingT TargetT::* InTargetMember)
			: Activations{ InActivations }, TargetMember{ InTargetMember } {}

		virtual bool CausesActivation(std::string_view Arg) const override
		{
			return Activations.contains(Arg);
		}

		virtual void Activate(TargetT& Target, std::string_view Arg, std::optional<std::string_view> Param) override
		{
			Target.*TargetMember = Activations[Arg];
		}
	};

	template <typename TargetT>
	struct ArgRuleBool : public ArgParamBase<TargetT>
	{
		std::vector<std::string_view> Activations;
		bool TargetT::* TargetMember;

		ArgRuleBool(std::vector<std::string_view>&& InActivations, bool TargetT::* InTargetMember)
			: Activations{ InActivations }, TargetMember{ InTargetMember } {}

		virtual bool CausesActivation(std::string_view Arg) const override
		{
			return std::ranges::find(Activations, Arg) != Activations.end();
		}

		virtual void Activate(TargetT& Target, std::string_view Arg, std::optional<std::string_view> Param) override
		{
			Target.*TargetMember = true;
		}
	};

	export
	template <typename OutputT>
	class ArgParser
	{
		std::vector<std::unique_ptr<ArgParamBase<OutputT>>> Rules;

	public:
		OutputT Parse(int argc, const char* argv[])
		{
			OutputT Output;

			for (int i{ 0 }; i < argc; ++i)
			{
				std::optional<std::string_view> Param;
				if (i + 1 < argc)
					Param = argv[i + 1];

				for (auto& Rule : Rules)
					Rule->Evaluate(Output, argv[i], Param);
			}

			return Output;
		}

		// Rules for processing floats, ints, string_view, path, etc.
		// These rules activate with one arg and then use the next arg as a parameter
		//TODO: add more types. can this be even more general?
		template <typename TargetMemberT>
		void AddRule(std::vector<std::string_view>&& Activations, TargetMemberT OutputT::* TargetMember)
		{
			Rules.push_back(std::make_unique<ArgRuleParam<OutputT, TargetMemberT>>(std::move(Activations), TargetMember));
		}

		// Rules for processing templated types
		template <template<typename...> class TargetMemberT, typename ArgT>
		void AddRule(std::vector<std::string_view>&& Activations, TargetMemberT<ArgT> OutputT::* TargetMember)
		{
			Rules.push_back(std::make_unique<ArgRuleParamContainer<OutputT, TargetMemberT, ArgT>>(std::move(Activations), TargetMember));
		}

		// Rules for processing flagged enums. Sets the value to the map value associated with the activated key
		template <typename TargetMemberT>
		void AddRule(std::unordered_map<std::string_view, TargetMemberT>&& Activations, TargetMemberT OutputT::* TargetMember)
		{
			Rules.push_back(std::make_unique<ArgRuleFlags<OutputT, TargetMemberT>>(std::move(Activations), TargetMember));
		}

		// Rules for processing bools. Assumes the bool is false by default, sets it true if activated
		void AddRule(std::vector<std::string_view>&& Activations, bool OutputT::* TargetMember)
		{
			Rules.push_back(std::make_unique<ArgRuleBool<OutputT>>(std::move(Activations), TargetMember));
		}
	};
}
