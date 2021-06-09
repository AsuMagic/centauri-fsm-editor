#pragma once

#include <cstddef>
#include <functional>

#include "../util/imgui.hpp"

namespace fsme
{
namespace detail
{

/**
 * @brief A helper to inherit from in specializations of std::hash for types that can be explicitly converted to
 *        std::size_t.
 */
template<class T>
struct id_hash_helper
{
	std::size_t operator()(const T& value) const noexcept
	{
		return std::size_t(value);
	}
};

}
}

namespace std
{

template<> struct hash<ed::NodeId> : fsme::detail::id_hash_helper<ed::NodeId> {};
template<> struct hash<ed::PinId> : fsme::detail::id_hash_helper<ed::PinId> {};
template<> struct hash<ed::LinkId> : fsme::detail::id_hash_helper<ed::LinkId> {};

}
