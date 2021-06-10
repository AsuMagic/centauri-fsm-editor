#pragma once

#include "../fwd.hpp"
#include "../visitor.hpp"
#include "../util/nativeformat.hpp"
#include "../util/imgui.hpp"

#include <algorithm>
#include <istream>
#include <memory>
#include <string>

namespace fsme
{
namespace visitors
{

/**
 * @brief Visitor to help deserialize the FSM into the native editor format.
 * @see NativeSerializer
 */
class NativeDeserializer : public NodeVisitor
{
public:
	static void deserialize(FsmEditor& editor, std::istream& input);

	void visit(nodes::CondNode& node) override;
	void visit(nodes::IfNode& node) override;
	void visit(nodes::StateNode& node) override;

private:
	NativeDeserializer(FsmEditor& editor, std::istream& input);

	void read(widgets::BoolExpressionInput& expression);

	void expect_magic(std::uint32_t magic);

	template<class T>
	T read_memcpy()
	{
		T ret;
		m_in->read(reinterpret_cast<char*>(&ret), sizeof(ret));
		return ret;
	}

	template<class T>
	void read_char_array(T& container)
	{
		std::size_t size = read_memcpy<std::uint64_t>();
		std::fill(container.begin(), container.end(), '\0');

		if (container.size() < size)
		{
			throw std::runtime_error("Failed to deserialize: Serialized buffer size is larger than expected");
		}

		m_in->read(container.data(), size);
	}

	std::string read_string();

	template<class T, class Func>
	void read_container(T& container, const Func& element_writer)
	{
		const std::size_t element_count = read_memcpy<std::uint64_t>();

		for (std::size_t i = 0; i < element_count; ++i)
		{
			element_writer();
		}
	}

	std::unique_ptr<Node> make_node_from_type(ed::NodeId id, native_format::NodeType node_type);

	FsmEditor* m_editor;
	std::istream* m_in;
};

}
}
