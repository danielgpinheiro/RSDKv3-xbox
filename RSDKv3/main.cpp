#include "RetroEngine.hpp"

#if !RETRO_USE_ORIGINAL_CODE

#if RETRO_PLATFORM == RETRO_WIN && _MSC_VER
#include "Windows.h"
#endif

#if RETRO_PLATFORM == RETRO_XBOX
static int SCREEN_WIDTH;
static int SCREEN_HEIGHT;

void SetXboxResolution() {
    // Based on LithiumX solution to detect Xbox resolution: https://github.com/Ryzee119/LithiumX/blob/f4471d287d44abc84803d3b901bd4aa7ed459689/src/platform/xbox/platform.c#L99
    // First try 720p. This is the preferred resolution
    SCREEN_WIDTH = 1280;
    SCREEN_HEIGHT = 720;
    if (XVideoSetMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, REFRESH_DEFAULT) == false)
    {
        // Fall back to 640*480
        SCREEN_WIDTH = 640;
        SCREEN_HEIGHT = 480;
        if (XVideoSetMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, REFRESH_DEFAULT) == false)
        {
            // Try whatever else the xbox is happy with
            VIDEO_MODE xmode;
            void *p = NULL;
            while (XVideoListModes(&xmode, 0, 0, &p))
            {
                if (xmode.width == 1080)
                    continue;
                if (xmode.width == 720)
                    continue; // 720x480 doesnt work on pbkit for some reason
                XVideoSetMode(xmode.width, xmode.height, xmode.bpp, xmode.refresh);
                ;
                break;
            }

            SCREEN_WIDTH = xmode.width;
            SCREEN_HEIGHT = xmode.height;
        }
    }
}
#endif

void parseArguments(int argc, char *argv[])
{
    for (int a = 0; a < argc; ++a) {
        const char *find = "";

        find = strstr(argv[a], "stage=");
        if (find) {
            int b = 0;
            int c = 6;
            while (find[c] && find[c] != ';') Engine.startSceneFolder[b++] = find[c++];
            Engine.startSceneFolder[b] = 0;
        }

        find = strstr(argv[a], "scene=");
        if (find) {
            int b = 0;
            int c = 6;
            while (find[c] && find[c] != ';') Engine.startSceneID[b++] = find[c++];
            Engine.startSceneID[b] = 0;
        }

        find = strstr(argv[a], "console=true");
        if (find) {
            engineDebugMode       = true;
            Engine.devMenu        = true;
            Engine.consoleEnabled = true;
#if RETRO_PLATFORM == RETRO_WIN && _MSC_VER
            AllocConsole();
            freopen_s((FILE **)stdin, "CONIN$", "w", stdin);
            freopen_s((FILE **)stdout, "CONOUT$", "w", stdout);
            freopen_s((FILE **)stderr, "CONOUT$", "w", stderr);
#endif
        }

        find = strstr(argv[a], "usingCWD=true");
        if (find) {
            usingCWD = true;
        }
    }
}
#endif

int main(int argc, char *argv[])
{
#if RETRO_PLATFORM == RETRO_XBOX
    SetXboxResolution();
#endif

#if !RETRO_USE_ORIGINAL_CODE
    parseArguments(argc, argv);
#endif

    debugPrint("Starting Retro Engine v3 ... Please Wait \n");
    Engine.Init();
    Engine.Run();

#if !RETRO_USE_ORIGINAL_CODE
    if (Engine.consoleEnabled) {
#if RETRO_PLATFORM == RETRO_WIN && _MSC_VER
        FreeConsole();
#endif
    }
#endif

    return 0;
}
