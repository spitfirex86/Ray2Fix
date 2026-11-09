# Ray2Fix

Ray2Fix is a collection of fixes and a configuration tool that aims to simplify setting up Rayman 2 for speedrunners.

**Note:** Mod loading functionality has been moved to a separate project: [**Twofold**](https://github.com/spitfirex86/Twofold), and is no longer included in Ray2Fix.

## Experimental open-source Glide backend (GameFrame)

This pull request replaces the proprietary dgVoodoo2 Glide 2.x runtime with
[GameFrame](https://github.com/tanepiper/game-frame), an Apache-2.0 licensed
Glide-to-OpenGL wrapper. GitHub Actions builds the pinned GameFrame source as a
32-bit `glide2x.dll`, preserving the original Rayman 2 GLI driver and the
Ray2Fix graphics/input patches. The experimental build no longer requires or
writes `dgVoodoo.conf`; existing resolution and window/timing settings
remain in `Ubi.ini` under the original `[Ray2Fix]` section.

**Not yet gameplay validated.** The GameFrame implementation currently uses
one texture unit (TMU 0), and its window management and 60 FPS pacing may
behave differently from dgVoodoo2. Speedrun eligibility checks are
intentionally disabled in this experiment pending timing validation.
Before testing, back up your installed `glide2x.dll` and `dgVoodoo.conf`
outside the game directory. Download the `Ray2Fix-devel` artifact from the
pull-request Actions build, not the upstream stable release.
See [GameFrame acceptance tests](docs/gameframe-validation.md), covering
Windows, Wine/Proton, widescreen, FOV, textures, videos, Alt+Tab and timing.

## Installation

#### Game versions:
- Recommended: The [GOG.com version](https://www.gog.com/game/rayman_2_the_great_escape) of the game installed using the **standalone installer**.
    - GOG Galaxy is not recommended, but should work as long as you do not let it overwrite the files after installing Ray2Fix.
- Uplay/Ubisoft Connect is not officially supported, although several users have reported successfully using it with Ray2Fix. Try at your own risk.
- Retail CD/DVD versions are not supported, as they use a different executable that includes DRM.

#### Guide:
- Download the **[latest release zip](https://github.com/spitfirex86/Ray2Fix/releases/latest)** and extract all files to the Rayman 2 installation directory.
- Run `R2FixCfg.exe` and choose your preferred display settings in the General tab.
- (Optional) Customize the controller layout in the Gamepad tab.

#### Controller troubleshooting:
- Try a different controller backend in R2FixCfg > Input.
- If using the SDL3 backend, make sure any other tools (such as DS4Windows) are disabled.
- Make sure only one controller is plugged in (including virtual controllers).

## Acknowledgements

- Rib - [Xidi-SDL3-Plugin](https://github.com/RibShark/Xidi-SDL3-Plugin)
- samuelgr - [Xidi](https://github.com/samuelgr/Xidi)
- Dege - [dgVoodoo2](https://dege.freeweb.hu/dgVoodoo2/)
