#pragma once

#include "../fwd.hpp"
#include "../visitor.hpp"

#include <ostream>

namespace fsme
{
namespace visitors
{

/**
 * @brief Visitor to help serialize the FSM into the native editor format.
 * @see CentauriSerializer
 */
class NativeSerializer : public NodeVisitor
{
public:
	void visit(nodes::CondNode& node) override;
	void visit(nodes::IfNode& node) override;
	void visit(nodes::StateNode& node) override;

private:
	NativeSerializer(std::ostream& output);

	void serialize_node_header(const Node& node);
	void serialize_expression(widgets::BoolExpressionInput& expression);

	template<class T>
	void serialize_memcpy(const T& value)
	{
		m_out->write(reinterpret_cast<char*>(value), sizeof(value));
	}

	template<class T>
	void serialize_char_array(const T& container)
	{
		serialize_memcpy(std::uint64_t(container.size()));
		m_out->write(container.data(), container.size());
	}

	template<class T, class Func>
	void serialize_container(const T& container, const Func& element_serializer)
	{
		serialize_memcpy(std::uint64_t(container.size()));

		for (const auto& elem : container)
		{
			element_serializer(elem);
		}
	}

	std::ostream* m_out;
};

}
}
