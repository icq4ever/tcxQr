# tcxQr

QR code generation for [TrussC](https://github.com/TrussC-org/TrussC), wrapping
[Nayuki's QR Code generator library](https://www.nayuki.io/page/qr-code-generator-library)
([GitHub](https://github.com/nayuki/QR-Code-generator), MIT).

The upstream library does the encoding; this addon is a thin layer that makes it
pleasant to use from a TrussC sketch — it swallows the exceptions, exposes the
module grid as a simple `module(x, y)` lookup, and does the arithmetic for sizing
a symbol to fit a box on screen.

`qrcodegen.hpp` / `qrcodegen.cpp` are vendored **unchanged** in
`libs/qrcodegen/src/`, so a newer upstream release can be dropped in without
re-applying local edits.

## Features

- **Byte-mode text encoding** — URLs, Wi-Fi join strings, plain text, any UTF-8.
  The smallest symbol version that fits is chosen automatically (up to version 40).
- **All four error-correction levels** — `Low`, `Medium` (default), `Quartile`, `High`.
- **Renderer-agnostic** — the addon hands you a grid of dark/light modules and
  nothing else, so you can draw it with `drawRect`, blit it into an `Image`, write
  it to an SVG, or feed it to a shader.
- **No exceptions to catch** — over-long input surfaces as `ok() == false` rather
  than a throw.
- **Quiet-zone-safe lookups** — reads outside the symbol return light, so the
  margin needs no special casing in your draw loop.

## Install

Add it to your project's `addons.make`:

```
tcxQr
```

…then `trusscli update`. No external dependencies and no build config to write —
TrussC collects `src/` and `libs/*/src/` automatically.

## Quick start

```cpp
#include <TrussC.h>
#include "tcxQr.h"

using namespace tc;

tcx::qr::Code code("https://github.com/TrussC-org/TrussC");

if (code.ok()) {
    const int   quiet = 4;                      // required margin, in modules
    const float cell  = code.cellFor(420.0f, quiet);   // fit into a 420px box
    const float ox = 60.0f, oy = 60.0f;

    // Light background across the whole box (quiet zone included)
    setColor(1.0f);
    drawRect(ox, oy, 420.0f, 420.0f);

    // Dark modules, inset by the quiet zone
    setColor(0.0f);
    for (int y = 0; y < code.size(); ++y)
        for (int x = 0; x < code.size(); ++x)
            if (code.module(x, y))
                drawRect(ox + (quiet + x) * cell, oy + (quiet + y) * cell, cell, cell);
}
```

## API

Everything lives in `namespace tcx::qr`.

### `Code`

| Member | Description |
|---|---|
| `Code(const std::string& text, Ecc ecc = Ecc::Medium)` | Encodes immediately. Never throws. |
| `bool ok() const` | `false` if the text did not fit in a version-40 symbol. |
| `int size() const` | Width of the symbol in modules (21–177), or `0` on failure. |
| `bool module(int x, int y) const` | Is the module at `(x, y)` dark? Out-of-range reads are light. |
| `float cellFor(float boxSize, int quiet = 4) const` | Module size in pixels that fits the symbol plus a `quiet`-module margin into a `boxSize` box. Returns `0` on failure. |

### `Ecc`

`Ecc::Low` (~7% recoverable), `Ecc::Medium` (~15%, default), `Ecc::Quartile` (~25%),
`Ecc::High` (~30%).

Higher correction means a denser symbol for the same text — encoding
`https://github.com/TrussC-org/TrussC` gives a 29×29 symbol at `Medium` but 37×37 at
`High`. Reach for `High` when the code is printed small, shown on a screen that
people photograph at an angle, or partly covered by a logo; otherwise `Medium` is
the usual choice.

## Notes

**Encode once, not per frame.** The constructor does the real work. Build a `Code`
when the text changes and keep it around — don't construct one inside `draw()`.

**Leave the quiet zone.** A QR symbol is only reliably scannable with a 4-module
light margin on all sides. `cellFor()` reserves it for you; just remember to offset
your drawing by `quiet * cell` as in the example above.

**Contrast and orientation.** Dark modules on a light background is what scanners
expect. Inverted (light-on-dark) codes are read by some decoders and not others, so
don't rely on it.

**`Code` has no default constructor** — it encodes in its constructor. To swap the
contents later, hold it in a `std::optional` or `std::unique_ptr` and replace it,
which is what the example does.

## Example

[`example-qr/`](example-qr/) — draws a symbol scaled to fit a box, with keys `1`–`4`
to switch error-correction level and `SPACE` to cycle through sample payloads (a URL,
a Wi-Fi join string, and plain text). It shows the module count and computed cell
size, so you can watch the symbol get denser as correction level goes up.

```bash
cd example-qr
trusscli update
trusscli run
```

## License

MIT — see [LICENSES.md](LICENSES.md) for the addon and the bundled
qrcodegen license.
