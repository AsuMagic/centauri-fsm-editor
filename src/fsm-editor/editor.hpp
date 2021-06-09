#pragma once

#include <vector>
#include <unordered_map>
#include <SFML/Graphics/RenderTarget.hpp>

#include "node.hpp"
#include "widgets/boolexprinput.hpp"
#include "widgets/stringinput.hpp"
#include "visitors/noderenderer.hpp"
#include "visitors/nodemenurenderer.hpp"
#include "util/idhash.hpp"
#include "util/imgui.hpp"
#include "fwd.hpp"

namespace fsme
{

/**
 * @brief A pair of pins that typically represent a link within the FSM graph.
 */
struct PinPair
{
	/// @brief The source pin of a link, which is an output within its node.
	ed::PinId from;

	/// @brief The destination pin of a link, which is an input within its node.
	ed::PinId to;

	bool operator==(const PinPair& other) const
	{
		return std::tie(from, to) == std::tie(other.from, other.to);
	}
};

/**
 * @brief Link information, containing the identifier of a link and the relevant pins.
 */
struct LinkInfo
{
	ed::LinkId id;
	PinPair pins;

	bool operator==(const PinPair& other) const
	{
		return pins == other;
	}
};

/**
 * @brief Pin information, containing the identifier of its node and any links related to this pin.
 */
struct PinInfo
{
	ed::NodeId node_id;
	std::vector<LinkInfo> links;
};

/**
 * @brief FSM editor class, which handles the UI and manages all graph-related entities.
 */
class FsmEditor
{
public:
	friend class visitors::NativeSerializer;

	FsmEditor(sf::RenderTarget& target);
	~FsmEditor();

	void render();

	/**
	 * @brief Returns a unique identifier for entities within the editor.
	 * @details As a result, all entities have a unique identifier, and a pin will never have the same ID as a node,
	 * for instance. This makes all IDs unique and more suitable for serialization.
	 */
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

	/**
	 * @brief Creates a new pin, attached to a specific node.
	 * @see new_unique_id() for ID allocation details
	 */
	ed::PinId create_pin(ed::NodeId node);

	/**
	 * @brief Destroys information for a given pin from the graph.
	 * @warning This is insufficient to remove all information for a given pin, as Node stores lists of pin IDs which
	 * are NOT cleared by this.
	 */
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

	void set_autocomplete_provider(widgets::BoolExpressionAutocomplete* autocomplete_provider);
	widgets::BoolExpressionAutocomplete* get_autocomplete_provider() const;

private:
	void handle_item_creation();
	void handle_item_deletion();

	void refresh_selected_objects();

	void render_menu_bar();
	void render_canvas();

	void render_links();
	void render_popups();

	sf::RenderTarget* m_target;

	ed::EditorContext* m_context;

	widgets::BoolExpressionAutocomplete* m_autocomplete_provider;

	widgets::StringInput m_shared_input;

	visitors::NodeRenderer m_node_renderer;
	visitors::NodeMenuRenderer m_node_menu_renderer;

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

inline void FsmEditor::set_autocomplete_provider(widgets::BoolExpressionAutocomplete* autocomplete_provider)
{
	m_autocomplete_provider = autocomplete_provider;
}

inline widgets::BoolExpressionAutocomplete* FsmEditor::get_autocomplete_provider() const
{
	return m_autocomplete_provider;
}

}
