#include <hal/debug.h>
#include <hal/video.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <windows.h>
#include <stdbool.h>
#include <nxdk/mount.h>
#include <zlib.h>

static const char *HBSX_VERSION = "Ver.0.0a";

static const int SCREEN_WIDTH = 640;
static const int SCREEN_HEIGHT = 480;

int main(void) {
  debugPrint("HBSX - %s\n\n", HBSX_VERSION);

  //Initialize stuff
  int initialized_SDL   = -1;
  int initialized_TTF   = -1;
  int initialized_PNG   = -1;
  bool ret;
  SDL_Window   *window   = NULL;
  SDL_Renderer *renderer = NULL;
  SDL_Texture  *bgTexture  = NULL;
  SDL_Texture  *logoTexture  = NULL;
  SDL_Texture  *verTexture  = NULL;
  SDL_Event event;

  //Set video mode
  XVideoSetMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, REFRESH_DEFAULT);
  
  //Initialize SDL
  initialized_SDL = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
  if (initialized_SDL != 0) {
    debugPrint("SDL_Init failed: %s", SDL_GetError());
    Sleep(2000);
    goto cleanup;
  }

  SDL_GameController *pad = NULL;

  //Initialize image (PNG)
  initialized_PNG = IMG_Init(IMG_INIT_PNG);
  if (!(initialized_PNG & IMG_INIT_PNG)) {
    debugPrint("IMG_Init failed: %s", IMG_GetError());
    Sleep(2000);
    goto cleanup;
  }

  //Initialize font (TTF)
  initialized_TTF = TTF_Init();
  if (initialized_TTF != 0) {
    debugPrint("TTF_Init failed: %s", TTF_GetError());
    Sleep(2000);
    goto cleanup;
  }

  ret = nxMountDrive('E', "\\Device\\Harddisk0\\Partition1\\");
  if (!ret) {
    debugPrint("Failed to mount E: drive!\n");
    Sleep(2000);
    goto cleanup;
  }

  // Retrieve drive bitmaks. Every bit represents one drive letter
  DWORD driveBits = GetLogicalDrives();
  if (driveBits == 0 && GetLastError() != ERROR_SUCCESS) {
    debugPrint("Failed to retrieve drive bitmask!\n");
    Sleep(5000);
    return 1;
  }
  debugPrint("Drive bitmask: 0x%lx\n\n", driveBits);

  SDL_Surface  *surface, *bgSurface, *logoSurface, *verSurface, *contentSurface;
  TTF_Font     *fontVersion;

  //Create window
  window = SDL_CreateWindow("HBSX",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            SCREEN_WIDTH, SCREEN_HEIGHT,
                            SDL_WINDOW_SHOWN);
  if (window == NULL) {
    debugPrint("Window creation failed: %s", SDL_GetError());
    Sleep(2000);
    goto cleanup;
  }

  renderer = SDL_CreateRenderer(window, -1, 0);
  if (renderer == NULL) {
    debugPrint("CreateRenderer failed: %s\n", SDL_GetError());
    Sleep(2000);
    goto cleanup;
  }

  //Add background
  if (SCREEN_HEIGHT == 480) {
    bgSurface = IMG_Load("D:\\background-480.png");
    if (!bgSurface) {
      debugPrint("Couldn't load image: %s", IMG_GetError());
      Sleep(2000);
      goto cleanup;
    }
  } else {
    bgSurface = IMG_Load("D:\\background.png");
    if (!bgSurface) {
      debugPrint("Couldn't load image: %s", IMG_GetError());
      Sleep(2000);
      goto cleanup;
    }
  }

  bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
  SDL_FreeSurface(bgSurface);
  if (bgTexture == NULL) {
    debugPrint("Couldn't create background texture: %s\n", SDL_GetError());
    Sleep(2000);
    goto cleanup;
  }

  //Add background
  if (SCREEN_HEIGHT == 480) {
    logoSurface = IMG_Load("D:\\logo-480.png");
    if (!logoSurface) {
      debugPrint("Couldn't load image: %s", IMG_GetError());
      Sleep(2000);
      goto cleanup;
    }
  } else if (SCREEN_HEIGHT == 720) {
    logoSurface = IMG_Load("D:\\logo-720.png");
    if (!logoSurface) {
      debugPrint("Couldn't load image: %s", IMG_GetError());
      Sleep(2000);
      goto cleanup;
    }
  } else {
    logoSurface = IMG_Load("D:\\logo.png");
    if (!logoSurface) {
      debugPrint("Couldn't load image: %s", IMG_GetError());
      Sleep(2000);
      goto cleanup;
    }
  }

  /* SDL_Color font_color = {0x7F, 0xFF, 0x7F, 0xFF};
  logoSurface = TTF_RenderUTF8_Blended(font, "HBSX", font_color);
  TTF_CloseFont(font);
  if (surface == NULL) {
    debugPrint("TTF_RenderText failed: %s", TTF_GetError());
    Sleep(2000);
    goto cleanup;
  } */

  //Font preparation

  //Font paths
  const char* fontRegular_path = "D:\\vegur-regular.ttf";
  const char* fontBold_path = "D:\\vegur-bold.ttf";

  //Font sizes
  const int fontVersion_size = 20;

  //Loading
  fontVersion = TTF_OpenFont(fontRegular_path, fontVersion_size);
  if (fontVersion == NULL) {
    debugPrint("Couldn't load fontVersion: %s", TTF_GetError());
    Sleep(2000);
    goto cleanup;
  }

  SDL_Color fontColor_xbox = {0x7F, 0xFF, 0x7F, 0xFF};
  verSurface = TTF_RenderUTF8_Blended(fontVersion, HBSX_VERSION, fontColor_xbox);
  TTF_CloseFont(fontVersion);
  if (verSurface == NULL) {
    debugPrint("TTF_RenderText failed: %s", TTF_GetError());
    Sleep(2000);
    goto cleanup;
  }

  logoTexture = SDL_CreateTextureFromSurface(renderer, logoSurface);
  SDL_FreeSurface(logoSurface);
  if (logoTexture == NULL) {
    debugPrint("Couldn't create logo texture: %s\n", SDL_GetError());
    Sleep(2000);
    goto cleanup;
  }

  verTexture = SDL_CreateTextureFromSurface(renderer, verSurface);
  SDL_FreeSurface(verSurface);
  if (verTexture == NULL) {
    debugPrint("Couldn't create version texture: %s\n", SDL_GetError());
    Sleep(2000);
    goto cleanup;
  }

  SDL_Rect logoPosition;
  int window_height, window_width;
  SDL_GetWindowSize(window, &window_width, &window_height);
  SDL_QueryTexture(logoTexture, NULL, NULL, &logoPosition.w, &logoPosition.h);
  logoPosition.x = window_height / 50;
  logoPosition.y = window_height / 50;

  SDL_Rect verPosition;
  SDL_QueryTexture(verTexture, NULL, NULL, &verPosition.w, &verPosition.h);
  verPosition.x = window_height / 2.5;
  verPosition.y = window_height / 28;

  /* Main render loop */
  bool running = true;
  while (running) {
    /* Check for events */
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_CONTROLLERDEVICEADDED:
        SDL_GameController *new_pad = SDL_GameControllerOpen(event.cdevice.which);
        if (pad == NULL) {
          pad = new_pad;
        }
        break;
      case SDL_CONTROLLERDEVICEREMOVED:
        if (pad == SDL_GameControllerFromInstanceID(event.cdevice.which)) {
          pad = NULL;
        }
        break;
        SDL_GameControllerClose(SDL_GameControllerFromInstanceID(event.cdevice.which));
      case SDL_CONTROLLERBUTTONDOWN:
        if (event.cbutton.button == SDL_CONTROLLER_BUTTON_START) {
          pad = (SDL_GameControllerFromInstanceID(event.cdevice.which));
        }
        if ((SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_BACK) == 1) &&
            (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == 1) &&
            (SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_TRIGGERLEFT) >= 1) &&
            (SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) >= 1)) {
          running = false;
          goto cleanup;
        }
        break;
      case SDL_QUIT:
        running = false;
        debugPrint("Shutting down HSBX...\n");
        break;
      default:
        break;
      }
    }

    SDL_GameControllerUpdate();
 
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);

    //Clear window
    SDL_RenderClear(renderer);

    //Background
    SDL_RenderCopy(renderer, bgTexture, NULL, NULL);

    //Logo
    SDL_RenderCopy(renderer, logoTexture, NULL, &logoPosition);

    //Version number
    SDL_RenderCopy(renderer, verTexture, NULL, &verPosition);

    SDL_Rect r;
    r.w = window_height * 1.1;
    r.h = window_height / 1.35;
    r.x = window_width / 2 - (r.w / 2);
    r.y = window_height / 5.5;

    // Set render color to blue ( rect will be rendered in this color )
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    // Render rect
    SDL_RenderFillRect( renderer, &r );

    //Show everything
    SDL_RenderPresent(renderer);
  }

cleanup:
  if (bgTexture != NULL) {
    SDL_DestroyTexture(bgTexture);
  }
  if (logoTexture != NULL) {
    SDL_DestroyTexture(logoTexture);
  }
  if (verTexture != NULL) {
    SDL_DestroyTexture(verTexture);
  }
  if (renderer != NULL) {
    SDL_DestroyRenderer(renderer);
  }
  if (window != NULL) {
    SDL_DestroyWindow(window);
  }
  if (pad != NULL) {
    SDL_GameControllerClose(pad);
  }
  if (initialized_PNG == 0) {
    IMG_Quit();
  }
  if (initialized_TTF == 0) {
    TTF_Quit();
  }
  if (initialized_SDL == 0) {
    SDL_Quit();
  }
  
  return 0;
}
