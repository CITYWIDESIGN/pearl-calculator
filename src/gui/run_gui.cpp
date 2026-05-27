#include "run_gui.h"

#include "app/Application.h"

int runGui()
{
    pearl::gui::Application app;
    return app.run();
}
