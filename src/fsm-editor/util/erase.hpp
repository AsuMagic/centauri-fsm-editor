#pragma once

#include <algorithm>

namespace fsme
{
namespace detail
{

/**
 * @brief Erases any element from \p container that is equal to \p to_erase (using std::find).
 */
template<class T, class U>
void erase(T& container, const U& to_erase)
{
	const auto it = std::find(begin(container), end(container), to_erase);

	if (it != end(container))
	{
		container.erase(it);
	}
}

/**
 * @brief Erases any element from \p container that is equal to \p to_erase (using container.find()).
 */
template<class T, class U>
void map_erase(T& container, const U& to_erase)
{
	const auto it = container.find(to_erase);

	if (it != end(container))
	{
		container.erase(it);
	}
}

}
}
