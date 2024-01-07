export module Enum;

import std.core;

export namespace Enum
{
	template <typename... Ts>
	concept EnumParameterPack = std::is_same_v<Ts...> && (std::is_enum_v<Ts> && ...);

	template <typename T, typename... Ts>
	concept EnumValueAndParameterPack = EnumParameterPack<Ts...>
		&& std::is_same_v<T, std::tuple_element_t<0, std::tuple<Ts...>>>;

	template <typename... Ts>
		requires EnumParameterPack<Ts...>
	constexpr auto AppendFlags(Ts... ts)
	{
		using ReturnT = std::tuple_element_t<0, std::tuple<Ts...>>;
		return static_cast<ReturnT>((static_cast<std::uint64_t>(ts) | ...));
	}

	template <typename T>
		requires std::is_enum_v<T>
	constexpr bool ContainsFlag(T Value, T Flag)
	{
		return (static_cast<std::uint64_t>(Value) & static_cast<std::uint64_t>(Flag)) != 0;
	}

	template <typename T, typename... Ts>
		requires EnumValueAndParameterPack<T, Ts...>
	constexpr bool ContainsFlags(T Value, Ts... Flags)
	{
		return (ContainsFlag(Value, Flags) && ...);
	}

	//TODO: I'm sure there are a ton more util functions I'll end up needing eventually
}
