#include "stringinput.hpp"

#include "../common.hpp"

namespace fsme
{

StringInput::StringInput() :
	m_buffer{0}
{}

void StringInput::render(bool editable)
{
	if (editable)
	{
		if (!m_hint.empty())
		{
			ImGui::InputTextWithHint("", m_hint.c_str(), m_buffer.data(), m_buffer.size());
		}
		else
		{
			ImGui::InputText("", m_buffer.data(), m_buffer.size());
		}
	}
	else
	{
		ImGui::Text("%s", m_buffer.data());
	}
}

void StringInput::set_text(const std::string& value)
{
	std::copy(
		value.c_str(),
		value.c_str() + std::min(value.size() + 1, m_buffer.size()),
		m_buffer.data());
}

std::string StringInput::get_text() const
{
	return std::string(m_buffer.data());
}

void StringInput::set_hint(std::string value)
{
	m_hint = std::move(value);
}

}
