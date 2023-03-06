#pragma once

#include <Model.h>


class Application
{
private:
    bool _shouldStop;
    Model _model;

public:
    bool getShouldStop() const noexcept { return _shouldStop; }

    void setShouldStop() noexcept { _shouldStop = true; }

    Model& getModel() noexcept { return _model; }
};
