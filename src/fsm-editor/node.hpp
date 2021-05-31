#pragma once

#include <imgui-node-editor/imgui_node_editor.h>
#include <vector>
#include <onidev/core/span.h>

#include "common.hpp"
#include "visitor.hpp"

namespace fsme
{
class FsmEditor;

class Node
{
public:
	Node(FsmEditor& editor, ed::NodeId id);

	virtual ~Node();

	virtual void render_context_menu();

	ed::NodeId node_id() const;

	enum class PinType
	{
		Input,
		Output,
		External
	};

	FsmEditor& editor() const;

	virtual bool can_connect(ed::PinId from, ed::PinId to) const;

	PinType pin_type(ed::PinId id) const;

	od::gsl::span<const ed::PinId> inputs() const;
	od::gsl::span<const ed::PinId> outputs() const;

	virtual void accept(NodeVisitor& v) = 0;

protected:
	int pin_index_or_default(ed::PinId needle, const std::vector<ed::PinId>& haystack, int if_not_found = -1) const;
	int input_pin_index(ed::PinId id) const;
	int output_pin_index(ed::PinId id) const;

	void resize_pins(std::vector<ed::PinId>& pins, std::size_t to_size);
	std::vector<ed::PinId>::iterator erase_pin(std::vector<ed::PinId>& pins, std::vector<ed::PinId>::iterator it);

	std::vector<ed::PinId> m_inputs, m_outputs;

private:
	FsmEditor* m_editor;
	ed::NodeId m_id;
};

inline ed::NodeId Node::node_id() const
{
	return m_id;
}

inline FsmEditor& Node::editor() const
{
	return *m_editor;
}

inline bool Node::can_connect(ed::PinId from, ed::PinId to) const
{
	const PinType from_type = pin_type(from);
	const PinType to_type = pin_type(to);

	// Allow to connect the nodes only when connecting to another node
	return (from_type == PinType::External && to_type == PinType::Input)
		|| (from_type == PinType::Output && to_type == PinType::External);
}

inline Node::PinType Node::pin_type(ed::PinId id) const
{
	if (input_pin_index(id) != -1)
	{
		return PinType::Input;
	}

	if (output_pin_index(id) != -1)
	{
		return PinType::Output;
	}

	return PinType::External;
}

inline od::gsl::span<const ed::PinId> Node::inputs() const
{
	return m_inputs;
}

inline od::gsl::span<const ed::PinId> Node::outputs() const
{
	return m_outputs;
}
}
