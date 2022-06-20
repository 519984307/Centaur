/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 13/06/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#include "TraderApplication.hpp"
#include <fmt/core.h>
#include <memory>

int main(C_UNUSED int argc, C_UNUSED char *argv[])
{
    try
    {
        // Create the main application
        auto app = std::make_unique<bspot::TraderApplication>();

        // Start running
        app->run();

    } catch (const std::exception &ex)
    {
        fmt::print("\nEXCEPTION: {}\n", ex.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
