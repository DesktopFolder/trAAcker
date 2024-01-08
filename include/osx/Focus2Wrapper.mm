#include "FocusedApplicationFinder.hpp"

#import <AppKit/AppKit.h>
#import <AppKit/NSWorkspace.h>
#import <Foundation/Foundation.h>

ApplicationInfo osx::get_current_application()
{
    // NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    ApplicationInfo app{};

    NSRunningApplication* focusedApp;
    focusedApp = [[NSWorkspace sharedWorkspace] frontmostApplication];

    if (focusedApp == nullptr)
    {
        return app;
    }

    app.pid  = [focusedApp processIdentifier];
    app.name = std::string([[focusedApp localizedName] UTF8String]);
    app.exec = std::string([[[focusedApp executableURL] lastPathComponent] UTF8String]);

    // [pool release];
    return app;
}
