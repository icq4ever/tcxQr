// =============================================================================
// example-qr - QR code generation demo
// =============================================================================
// Encodes a string into a QR symbol and draws it as a grid of squares.
//
// Controls:
//   1-4    : error correction level (Low / Medium / Quartile / High)
//   SPACE  : cycle through sample payloads
// =============================================================================

#include "tcApp.h"

namespace {

const char* kSamples[] = {
    "https://github.com/TrussC-org/TrussC",
    "WIFI:T:WPA;S:studio-5g;P:hunter2;;",
    "hello, tcxQr",
};
int sampleIndex = 0;

const char* eccName(tcx::qr::Ecc e) {
    switch (e) {
        case tcx::qr::Ecc::Low:      return "Low (~7%)";
        case tcx::qr::Ecc::Medium:   return "Medium (~15%)";
        case tcx::qr::Ecc::Quartile: return "Quartile (~25%)";
        case tcx::qr::Ecc::High:     return "High (~30%)";
    }
    return "?";
}

}  // namespace

void tcApp::setup() {
    rebuild();

    logNotice("tcApp") << "tcxQr Example";
    logNotice("tcApp") << "  1-4   : error correction level";
    logNotice("tcApp") << "  SPACE : next sample payload";
}

void tcApp::rebuild() {
    // Encoding happens here, once — not every frame. A version-40 symbol is
    // ~1200 bytes of work, which is cheap, but it is still pointless to redo
    // it 60 times a second for a string that has not changed.
    code_ = std::make_unique<tcx::qr::Code>(text_, ecc_);
}

void tcApp::draw() {
    clear(0.11f);

    const float box = 420.0f;                    // square we want to fill
    const float ox  = 60.0f;
    const float oy  = 130.0f;

    if (!code_->ok()) {
        setColor(1.0f, 0.4f, 0.4f);
        drawBitmapString("encode failed - text too long for a QR symbol", ox, oy);
        return;
    }

    const int   quiet = 4;                       // required margin, in modules
    const float cell  = code_->cellFor(box, quiet);

    // The quiet zone is part of the symbol: draw the light background across
    // the whole box, then only the dark modules inset by `quiet` cells.
    setColor(1.0f);
    drawRect(ox, oy, box, box);

    setColor(0.0f);
    const float mx = ox + quiet * cell;
    const float my = oy + quiet * cell;
    for (int y = 0; y < code_->size(); ++y) {
        for (int x = 0; x < code_->size(); ++x) {
            if (code_->module(x, y)) {
                drawRect(mx + x * cell, my + y * cell, cell, cell);
            }
        }
    }

    // ---- info panel ---------------------------------------------------------
    const float tx = ox + box + 40.0f;
    setColor(1.0f);
    drawBitmapString("tcxQr", tx, oy);
    drawBitmapString("ECC      : " + std::string(eccName(ecc_)), tx, oy + 30);
    drawBitmapString("modules  : " + std::to_string(code_->size()) + " x "
                                   + std::to_string(code_->size()), tx, oy + 50);
    char cellText[64];
    snprintf(cellText, sizeof(cellText), "cell     : %.2f px", cell);
    drawBitmapString(cellText, tx, oy + 70);

    setColor(0.65f);
    drawBitmapString("1-4 ecc   SPACE sample", tx, oy + 110);

    setColor(0.9f);
    drawBitmapString("text:", 60, 50);
    setColor(0.55f, 0.85f, 1.0f);
    drawBitmapString(text_, 60, 72);
}

void tcApp::keyPressed(int key) {
    switch (key) {
        case '1': ecc_ = tcx::qr::Ecc::Low;      rebuild(); return;
        case '2': ecc_ = tcx::qr::Ecc::Medium;   rebuild(); return;
        case '3': ecc_ = tcx::qr::Ecc::Quartile; rebuild(); return;
        case '4': ecc_ = tcx::qr::Ecc::High;     rebuild(); return;
    }

    if (key == KEY_SPACE) {
        sampleIndex = (sampleIndex + 1) % (int)(sizeof(kSamples) / sizeof(kSamples[0]));
        text_ = kSamples[sampleIndex];
        rebuild();
    }
}
