#pragma once

#include <imgui.h>

#include <Application.h>


class Viewable
{
public:
    virtual void onImGuiRender(Application& app) = 0;
};
