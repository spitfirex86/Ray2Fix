#!/usr/bin/env python3
"""Ray2Fix-only adaptation to a pinned upstream GameFrame checkout."""
from pathlib import Path
import sys


def once(src: str, before: str, after: str, description: str) -> str:
    hits = src.count(before)
    if hits != 1:
        raise RuntimeError(f"{description}: expected one upstream anchor, got {hits}")
    return src.replace(before, after, 1)


def main(root: Path) -> None:
    code = root / "wrappers" / "glide" / "src"
    win = code / "win32.rs"
    src = win.read_text(encoding="utf-8")
    src = once(src, "    pub fn GetCurrentProcessId() -> u32;",
               "    pub fn GetCurrentProcessId() -> u32;\n"
               "    pub fn GetSystemMetrics(index: i32) -> i32;",
               "desktop dimensions")
    win.write_text(src, encoding="utf-8")

    window = code / "window.rs"
    src = window.read_text(encoding="utf-8")
    addition = """/// Ray2Fix windowed mode is a decorated window; fullscreen is borderless
/// on the primary monitor. Never request exclusive display mode changes.
fn ray2fix_set_window(hwnd: HWND, w: u32, h: u32) -> bool {
    if std::env::var("RAY2FIX_GAMEFRAME").as_deref() != Ok("1") {
        return false;
    }
    let windowed = std::env::var("RAY2FIX_GLIDE_WINDOWED").as_deref() == Ok("1");
    unsafe {
        let style = if windowed { 0x00cf_0000u32 | WS_VISIBLE }
                    else { 0x8000_0000u32 | WS_VISIBLE };
        SetWindowLongA(hwnd, GWL_STYLE, style as i32);
        let mut rect = RECT { left: 0, top: 0, right: w as i32, bottom: h as i32 };
        let (x, y, width, height) = if windowed {
            AdjustWindowRectEx(&mut rect, style, 0, GetWindowLongA(hwnd, GWL_EXSTYLE) as u32);
            let outer_w = rect.right - rect.left;
            let outer_h = rect.bottom - rect.top;
            let screen_w = GetSystemMetrics(0);
            let screen_h = GetSystemMetrics(1);
            ((screen_w - outer_w).max(0) / 2, (screen_h - outer_h).max(0) / 2, outer_w, outer_h)
        } else {
            // Render in the configured logical resolution, centered and
            // aspect-correct within the desktop-sized presentation window.
            (0, 0, GetSystemMetrics(0), GetSystemMetrics(1))
        };
        SetWindowPos(hwnd, null_mut(), x, y, width, height, SWP_NOZORDER | 0x20);
        log!("Ray2Fix GameFrame windowed={windowed} size={width}x{height} pos={x},{y}");
    }
    true
}

"""
    src = once(src, "pub fn prepare(w: u32, h: u32) -> HWND {",
               addition + "pub fn prepare(w: u32, h: u32) -> HWND {",
               "window helper")
    old_prepare = """    unsafe {
        // WS_POPUP | WS_VISIBLE: a borderless window at (0, 0), like the full screen it was.
        SetWindowLongA(hwnd, GWL_STYLE, (0x8000_0000u32 | WS_VISIBLE) as i32);
        let mut r = RECT { left: 0, top: 0, right: w as i32, bottom: h as i32 };
        AdjustWindowRectEx(&mut r, GetWindowLongA(hwnd, GWL_STYLE) as u32, 0, GetWindowLongA(hwnd, GWL_EXSTYLE) as u32);
        SetWindowPos(hwnd, null_mut(), 0, 0, r.right - r.left, r.bottom - r.top, SWP_NOZORDER | 0x20);
    }"""
    src = once(src, old_prepare,
               "    if !ray2fix_set_window(hwnd, w, h) {\n" +
               old_prepare.replace("\n", "\n    ") + "\n    }",
               "window prepare override")
    old_size = """    unsafe {
        SetWindowLongA(hwnd, GWL_STYLE, (0x8000_0000u32 | WS_VISIBLE) as i32);
        SetWindowPos(hwnd, null_mut(), 0, 0, w as i32, h as i32, SWP_NOZORDER | 0x20);
    }"""
    src = once(src, old_size,
               "    if !ray2fix_set_window(hwnd, w, h) {\n" +
               old_size.replace("\n", "\n    ") + "\n    }",
               "window size override")
    window.write_text(src, encoding="utf-8")

    # dgVoodoo previously capped at 60 FPS, independently from game timing.
    sst = code / "api" / "sst.rs"
    src = sst.read_text(encoding="utf-8")
    pacing = """/// Ray2Fix speedrun compatibility: at most 60 swaps per second.
fn ray2fix_frame_pace() {
    if std::env::var("RAY2FIX_GAMEFRAME").as_deref() != Ok("1") {
        return;
    }
    thread_local! {
        static LAST: std::cell::RefCell<Option<std::time::Instant>> =
            const { std::cell::RefCell::new(None) };
    }
    let frame = std::time::Duration::from_nanos(1_000_000_000 / 60);
    LAST.with(|state| {
        let mut last = state.borrow_mut();
        if let Some(previous) = *last {
            let deadline = previous + frame;
            let now = std::time::Instant::now();
            if now < deadline {
                std::thread::sleep(deadline - now);
            }
        }
        *last = Some(std::time::Instant::now());
    });
}

"""
    src = once(src, '#[unsafe(no_mangle)]\npub extern "system" fn grBufferSwap(_interval: u32) {',
               pacing + '#[unsafe(no_mangle)]\npub extern "system" fn grBufferSwap(_interval: u32) {',
               "frame pacing definition")
    src = once(src, "    shim::calls::end_frame();",
               "    shim::calls::end_frame();\n    ray2fix_frame_pace();",
               "frame pacing invocation")
    sst.write_text(src, encoding="utf-8")
    print("GameFrame patched for Ray2Fix window modes and 60 FPS cap")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        sys.exit("usage: patch_ray2fix.py <pinned-game-frame-checkout>")
    main(Path(sys.argv[1]))
