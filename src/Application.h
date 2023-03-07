#pragma once

#include <Model.h>


class Application
{
private:
    bool _shouldStop;
    Model _model;

public:
    bool getShouldStop() const noexcept { return _shouldStop; }
    Model& getModel() noexcept { return _model; }

    void setShouldStop() noexcept { _shouldStop = true; }
};
