# Ray2Fix

Ray2Fix is a collection of fixes and a configuration tool that aims to simplify setting up Rayman 2 for speedrunners.

It can also be used as a basic mod loader (can be enabled in Tweaks tab in R2FixCfg.exe).

## How to Install

#### Game versions:
- Recommended: The [GOG.com version](https://www.gog.com/game/rayman_2_the_great_escape) of the game installed using the [standalone installer](https://www.gog.com/downloads/rayman_2_the_great_escape/en1installer0).
- GOG Galaxy is not supported (just use the standalone installer)
- Retail CD/DVD versions are not supported (due to a different game EXE that includes DRM)
- Ubisoft Connect - investigating. Some report that it works, others that it does not. Try at your own risk.
#### Guide:
- Download the **[latest release zip](https://github.com/spitfirex86/Ray2Fix/releases/latest)** and extract all files to Rayman 2 installation directory.
- Run R2FixCfg.exe and choose your preferred display resolution.
- (Optional) Configure controller input using Steam (described below).


## Using Steam to map controller input

In Steam -> Big Picture -> Settings -> Controller settings:
  - Enable support for the correct controller type
  - Enable "Generic" configuration support

In Steam Library:
- Add Rayman2.exe as a non-Steam game
- Right-click Rayman 2 -> Manage -> Controller configuration

Example controller config: <steam://controllerconfig/rayman%202%20the%20great%20escape/2357726715>
(Note: the name of the game in Steam MUST be set to "Rayman 2: The Great Escape" in order to import above config!)

#### Controller troubleshooting:
- Make sure only one controller is plugged in (including virtual controllers).
- In some cases it may be necessary to disable Desktop Configuration in Big Picture Settings.
