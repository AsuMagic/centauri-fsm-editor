#include "node.hpp"

#include <algorithm>

#include "editor.hpp"

namespace fsme
{

Node::Node(FsmEditor& editor, ed::NodeId id) :
	m_editor(&editor),
	m_id(id)
{}

Node::~Node()
{
	resize_pins(m_inputs, 0);
	resize_pins(m_outputs, 0);
}

int Node::pin_index_or_default(ed::PinId needle, const std::vector<ed::PinId>& haystack, int if_not_found) const
{
	const auto it = std::find(haystack.begin(), haystack.end(), needle);
	return it != haystack.end() ? std::distance(haystack.begin(), it) : if_not_found;
}

int Node::input_pin_index(ed::PinId id) const
{
	return pin_index_or_default(id, m_inputs);
}

int Node::output_pin_index(ed::PinId id) const
{
	return pin_index_or_default(id, m_outputs);
}

void Node::resize_pins(std::vector<ed::PinId>& pins, std::size_t to_size)
{
	const std::size_t old_size = pins.size();

	// Unbind old IDs
	for (std::size_t i = to_size; i < old_size; ++i)
	{
		m_editor->destroy_pin(pins[i]);
	}

	pins.resize(to_size);

	// Allocate new IDs as required
	for (std::size_t i = old_size; i < to_size; ++i)
	{
		pins[i] = m_editor->create_pin(node_id());
	}
}

std::vector<ed::PinId>::iterator Node::erase_pin(std::vector<ed::PinId>& pins, std::vector<ed::PinId>::iterator it)
{
	m_editor->destroy_pin(*it);
	return pins.erase(it);
}

}
