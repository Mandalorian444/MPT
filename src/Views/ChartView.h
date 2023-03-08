#pragma once

#include <Views/Viewable.h>


class ChartView : public Viewable
{
public:
    void onImGuiRender(Application& app) override;
};
