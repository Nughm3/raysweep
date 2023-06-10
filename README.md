# raysweep

Minesweeper in C with [raylib](https://raylib.com). Made in 1 day (for fun).

There are some ~~bugs~~ *minor differences* with the actual game (non-exhaustive):

- Opening squares happens on click, not on release
- Cascading has some weird edge case behavior
- Clicks aren't debounced:
  - Clicking to restart a game can perform the first click
  - Sometimes opening a square will accidentally chord it since the mouse press is registered multiple times
- Got a segfault once and I'm not sure if it's fixed

## Build

With [raylib](https://raylib.com), [Meson](https://mesonbuild.com) & [Ninja](https://ninja-build.org):

```bash
git clone https://github.com/Nughm3/raysweep
meson setup build
meson compile -C build
```

With Nix (flakes):

```bash
nix profile install github:Nughm3/raysweep
```

### LICENSE

Licensed under the [Unlicense](/LICENSE).
