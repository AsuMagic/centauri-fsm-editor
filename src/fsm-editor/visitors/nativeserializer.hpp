#pragma once

#include "../fwd.hpp"
#include "../visitor.hpp"
#include "../util/nativeformat.hpp"

#include <ostream>

namespace fsme
{
namespace visitors
{

/**
 * @brief Visitor to help serialize the FSM into the native editor format.
 * @see CentauriSerializer
 * @see NativeDeserializer
 */
class NativeSerializer : public NodeVisitor
{
public:
	static void serialize(FsmEditor& editor, std::ostream& output);

	void visit(nodes::CondNode& node) override;
	void visit(nodes::IfNode& node) override;
	void visit(nodes::StateNode& node) override;

private:
	NativeSerializer(std::ostream& output);

	void write(const Node& node);
	void write(widgets::BoolExpressionInput& expression);

	template<class T>
	void write_memcpy(const T& value)
	{
		m_out->write(reinterpret_cast<const char*>(&value), sizeof(value));
	}

	template<class T>
	void write_string(const T& container)
	{
		write_memcpy(std::uint64_t(container.size()));
		m_out->write(container.data(), container.size());
	}

	template<class T, class Func>
	void write_container(const T& container, const Func& element_serializer)
	{
		write_memcpy(std::uint64_t(container.size()));

		for (const auto& elem : container)
		{
			element_serializer(elem);
		}
	}

	std::ostream* m_out;
};

}
}
