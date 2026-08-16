# barq ⚡ برق

*Barq* is Arabic for lightning — and a **barqiyya** (برقية) was the telegram, history's
fastest written note. This barq is a floating scratch note for your desktop that
appears instantly and always saves itself.

Someone calls, you need to write something down *now*: hit the keybinding, type,
hit it again. No file to name, no save button, no app to wait for. Whatever you
wrote is there the next time you summon it — even after a reboot.

Built with Qt Quick in the spirit of [Omawrite](https://github.com/omacom-io/omawrite):
one small binary, no vaults, no plugins.

## How it works

- **One note, always saved.** Everything you type is auto-saved (atomically) to
  `~/.local/share/barq/note.md` a few hundred milliseconds after you stop typing,
  and again the moment the window hides. The ⚡ in the corner means "saved".
- **Instant summon.** The first launch stays resident; every later `barq`
  invocation just toggles the running window. Bind `barq` to a key and it
  behaves like a show/hide scratchpad.
- **Esc or closing the window hides it** (the note is put away, not the app).
  `Ctrl+Q` really quits.
- **Theme-synced.** On [Omarchy](https://omarchy.org) it follows your current
  theme's colors live, plus the desktop's dark mode and text scale everywhere
  else via the desktop portal.

## Install

### Arch / Omarchy

```sh
git clone https://github.com/SL0wZEr/barq.git
cd barq
./bin/install   # builds and installs via makepkg
```

Dependencies: `qt6-base`, `qt6-declarative`, `xdg-desktop-portal`.

### Build only

```sh
./bin/build     # produces build/barq
```

## Omarchy setup

Add a summon key in `~/.config/hypr/bindings.lua`:

```lua
o.bind("SUPER + B", "Barq note", "barq")
```

(Bind the plain command, not `{ launch = "barq" }` — the launch wrapper adds
~150ms of process scoping to every toggle.)

And make it float in `~/.config/hypr/hyprland.lua` (below the Omarchy defaults):

```lua
o.window("barq", { float = true, center = true })
```

No size rule needed: barq sizes itself to ~36% × 45% of the screen (capped at
640 × 520), so it keeps the same apparent size at any monitor scale.

Tip: add `pin = true` to the window rule and the note follows you across
workspaces — handy mid-phone-call.

## Plain Hyprland

```conf
bind = SUPER, B, exec, barq
windowrulev2 = float, class:^(barq)$
windowrulev2 = center, class:^(barq)$
```

## Credits

Architecture and desktop-portal theme syncing adapted from
[Omawrite](https://github.com/omacom-io/omawrite) (MIT, DHH / Omacom).

## License

[MIT](LICENSE)
