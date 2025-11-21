#pragma once

#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"

namespace Menu
{
	extern ImGuiWindowFlags m_iWindowFlags;
	extern bool m_bShowDemo;

	void RenderMenu();
}