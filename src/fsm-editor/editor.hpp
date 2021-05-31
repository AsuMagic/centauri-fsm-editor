#pragma once

#include <imgui-node-editor/imgui_node_editor.h>
#include <vector>
#include <unordered_map>

#include "common.hpp"
#include "node.hpp"
#include "widgets/boolexprinput.hpp"
#include "visitors/noderenderer.hpp"

namespace fsme
{

struct PinPair
{
	ed::PinId start, end;

	bool operator==(const PinPair& other) const
	{
		return std::tie(start, end) == std::tie(other.start, other.end);
	}
};

struct LinkInfo
{
	ed::LinkId id;
	PinPair pins;

	bool operator==(const PinPair& other) const
	{
		return pins == other;
	}
};

struct PinInfo
{
	ed::NodeId node_id;
	std::vector<LinkInfo> links;
};

class FsmEditor
{
public:
	FsmEditor();
	~FsmEditor();

	void render();

	std::size_t new_unique_id();

	template<class NodeType>
	NodeType& make_node()
	{
		const std::size_t id = new_unique_id();
		auto ptr = std::make_unique<NodeType>(*this, id);
		auto& node = *ptr;

		m_nodes.emplace(std::make_pair(id, std::move(ptr)));

		return node;
	}
	void destroy_node(ed::NodeId id);

	ed::PinId create_pin(ed::NodeId node);
	void destroy_pin(ed::PinId pin);
	const PinInfo* get_pin_info(ed::PinId pin) const;

	ed::LinkId create_link(PinPair pin_pair);
	void destroy_link(ed::LinkId link);
	const PinPair* get_link_info(ed::LinkId link) const;

	void destroy_links_involving(ed::PinId pin);

	Node* get_node_by_id(ed::NodeId id) const;
	Node* get_node_by_pin_id(ed::PinId pin) const;

	bool is_link_selected(ed::LinkId link) const;
	bool is_node_selected(ed::NodeId node) const;

	void set_autocomplete_provider(BoolExpressionAutocomplete* autocomplete_provider);
	BoolExpressionAutocomplete* get_autocomplete_provider() const;

private:
	void handle_item_creation();
	void handle_item_deletion();

	void refresh_selected_objects();

	void render_links();
	void render_popups();

	ed::EditorContext* m_context;

	BoolExpressionAutocomplete* m_autocomplete_provider;

	NodeRenderer m_node_renderer;

	std::size_t m_last_allocated_id;

	ed::NodeId m_context_menu_node;
	ed::PinId m_context_menu_pin;
	ed::LinkId m_context_menu_link;

	std::vector<ed::LinkId> m_selected_links;
	std::vector<ed::NodeId> m_selected_nodes;

	std::unordered_map<ed::PinId, PinInfo> m_pin_infos;
	std::unordered_map<ed::LinkId, PinPair> m_link_infos;
	std::unordered_map<ed::NodeId, std::unique_ptr<Node>> m_nodes;
};

inline std::size_t FsmEditor::new_unique_id()
{
	return ++m_last_allocated_id;
}

inline void FsmEditor::set_autocomplete_provider(BoolExpressionAutocomplete* autocomplete_provider)
{
	m_autocomplete_provider = autocomplete_provider;
}

inline BoolExpressionAutocomplete* FsmEditor::get_autocomplete_provider() const
{
	return m_autocomplete_provider;
}

}
