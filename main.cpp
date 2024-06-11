#include "src/Application.hpp"
#include "logging.hpp"

int main()
{   
    // Leave this at the start of main to ensure correct shutdown order.
    // Um... sort of dumb, but I believe in 'dumb but works until I fix it'
    // as opposed to 'leaving a gun on the table with the safety off'.
    // This is for logging - we preload the list of files object.
    // That way we close files at the very end.
    const auto& _ = aa::detail::get_files();

    aa::Application app;

    try
    {
        app.run();
    }
    catch (const std::exception& e)
    {
        aa::log::error("Encountered fatal error: ", e.what());
        throw;
    }
    return 0;
}