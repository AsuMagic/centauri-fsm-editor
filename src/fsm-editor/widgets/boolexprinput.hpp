#pragma once

#include <array>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include <istream>

namespace fsme
{
namespace widgets
{

struct BoolExpressionOption
{
	std::string shorthand;
	std::string lua_expression;

	const char* get_main_text() const
	{
		return !shorthand.empty() ? shorthand.c_str() : lua_expression.c_str();
	}

	const char* get_help_text() const
	{
		return !shorthand.empty() ? lua_expression.c_str() : nullptr;
	}
};

struct BoolExpressionCategory
{
	std::vector<BoolExpressionOption> options;
};

class BoolExpressionAutocomplete
{
	public:
	struct FilterOptions
	{
		std::set<const BoolExpressionOption*>* selected_already;
	};

	void add_option(const std::string& category, BoolExpressionOption&& option);
	const BoolExpressionOption* render(FilterOptions options);

	private:
	std::unordered_map<std::string, BoolExpressionCategory> m_categories;
};

inline void BoolExpressionAutocomplete::add_option(const std::string& category, BoolExpressionOption&& option)
{
	m_categories[category].options.emplace_back(std::move(option));
}

enum class ExpressionInputType
{
	PlainLuaExpression,
	SimpleExpression
};

struct PlainLuaInput
{
	std::array<char, 4096> text_buffer = {0};
};

struct SimpleExpressionInput
{
	std::set<const BoolExpressionOption*> options;

	std::string text_preview() const;
};

class BoolExpressionInput
{
public:
	BoolExpressionInput(std::size_t id);

	void set_autocomplete_provider(BoolExpressionAutocomplete* autocomplete_provider);

	void input_render(bool editable);
	void popup_render();

	std::string as_lua_expression() const;

	ExpressionInputType get_input_type() const { return m_input_type; }
	void set_input_type(ExpressionInputType type) { m_input_type = type; }

	PlainLuaInput& get_raw_lua_input() { return m_lua_input; }
	SimpleExpressionInput& get_raw_simple_expression_input() { return m_expr_input; }

	std::size_t get_id() const;

private:
	BoolExpressionAutocomplete* m_autocomplete_provider = nullptr;

	std::size_t m_id;

	ExpressionInputType m_input_type = ExpressionInputType::SimpleExpression;
	PlainLuaInput m_lua_input;
	SimpleExpressionInput m_expr_input;
};

inline void BoolExpressionInput::set_autocomplete_provider(BoolExpressionAutocomplete* autocomplete_provider)
{
	m_autocomplete_provider = autocomplete_provider;
}

inline std::size_t BoolExpressionInput::get_id() const
{
	return m_id;
}

}
}
