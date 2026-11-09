# Ray2Fix + GameFrame: experimental acceptance tests

This PR replaces the proprietary dgVoodoo2 Glide2x backend with a reproducible
32-bit [GameFrame](https://github.com/tanepiper/game-frame) build.

## Before testing

- Use a clean copy of Rayman 2 from a supported edition; keep the current
  stable installation intact. Do not overwrite an existing Glide2x DLL without
  backing it up, preferably outside the game directory.
- Download the **Ray2Fix-devel** artifact from this PR's successful build.
  Copy its contents to the game directory.
- Check that the package includes: `glide2x.dll`,
  `DLL/GliFixVf.dll` (or the correct project layout),
  `R2FixCfg.exe` and `GameFrame-LICENSE-APACHE.txt`.
- Keep the previous `dgVoodoo.conf` as a rollback reference; it is not
  required by the new backend.
- Under Wine, install 32-bit OpenGL support and use a native DLL override for
  Glide2x if necessary (e.g. `WINEDLLOVERRIDES="glide2x=n"`).

## Windows 11 and Wine/Proton

Repeat on both platforms and attach relevant logs/screenshots for regressions:

1. Start Ray2Fix. Confirm file verification succeeds without dgVoodoo.conf
   and that the configuration tool enables Ray2Fix.
2. Start at 1024 x 768 windowed. Check title bar, correct aspect, centering,
   mouse/controller focus, video playback, game menu and level load.
3. Change to desktop-resolution borderless fullscreen. Check no exclusive
   display-mode switch, preserved aspect and Alt+Tab away/back.
4. Test 1280 x 1024 and 1920 x 1080 with the widescreen patch on/off.
   Check FOV, HUD, menus, fonts, fades, transitions and image edges.
5. Check long gameplay (at least one complete level), cutscenes, save/load,
   gamepad mapping, screenshot creation/deletion and clean exit.
6. Inspect frame pacing at 60 FPS on a 60 Hz and high-refresh-rate display.
   Test 30 Hz game timing separately. Report stalls or timing deviations.
7. Remove `dgVoodoo.conf`, reopen Ray2Fix and confirm the settings persist
   in `Ubi.ini`. Confirm no file from dgVoodoo2 gets rewritten.

## Known architectural differences

- GameFrame currently only renders TMU 0; games requiring a second texture
  unit may differ from dgVoodoo2. No Rayman 2 rendering claim until tested.
- VSync and exclusive fullscreen controls from dgVoodoo2 are not available.
- This PR adds a Ray2Fix-only adapter to GameFrame's window management and
  an experimental 60 FPS limit. Timing must be validated for speedrunning.
- The CI tests PE32 architecture and decorated exports, **not gameplay**.

Once both platform tests are accepted, decide whether this branch can be
marked ready for review upstream.
