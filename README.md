# Ray2Fix

Ray2Fix is a collection of fixes and a configuration tool that aims to simplify setting up Rayman 2 for speedrunners.

**Note:** Mod loading functionality has been moved to a separate project: [**Twofold**](https://github.com/spitfirex86/Twofold), and is no longer included in Ray2Fix.

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
- 