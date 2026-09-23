#pragma once

// =============================================================================
// tcxQr - QR code generator
// =============================================================================
//
// Addon for TrussC. Add "tcxQr" to addons.make to use.
//
// Thin wrapper over Nayuki's qrcodegen (MIT), vendored unchanged in
// libs/qrcodegen/src/. Keeping the upstream files untouched means a newer
// version can be dropped in without re-applying edits.
//
// Usage example:
//   tcx::qr::Code code("https://example.com/breath");
//   if (code.ok()) {
//       const float cell = 8.0f;
//       for (int y = 0; y < code.size(); ++y)
//           for (int x = 0; x < code.size(); ++x)
//               if (code.module(x, y))
//                   tc::drawRect(ox + x * cell, oy + y * cell, cell, cell);
//   }
//
// Leave a quiet zone of 4 modules around the symbol or scanners may fail —
// cellFor() below sizes a module so the symbol plus its quiet zone fits a box.
//
// Error correction defaults to Medium (~15% recoverable). Use High when the
// code is printed small, shown on a screen people photograph at an angle, or
// partly covered by a logo.
// =============================================================================

#include <optional>
#include <string>

#include "qrcodegen.hpp"

namespace tcx::qr {

enum class Ecc { Low, Medium, Quartile, High };

class Code {
public:
    // ok() is false when the text is too long to fit in a version-40 symbol.
    explicit Code(const std::string& text, Ecc ecc = Ecc::Medium) {
        using qrcodegen::QrCode;
        QrCode::Ecc level = QrCode::Ecc::MEDIUM;
        switch (ecc) {
            case Ecc::Low:      level = QrCode::Ecc::LOW;      break;
            case Ecc::Medium:   level = QrCode::Ecc::MEDIUM;   break;
            case Ecc::Quartile: level = QrCode::Ecc::QUARTILE; break;
            case Ecc::High:     level = QrCode::Ecc::HIGH;     break;
        }
        try {
            code_ = QrCode::encodeText(text.c_str(), level);
            size_ = code_->getSize();
        } catch (...) {
            // data too long, or the text could not be encoded
            size_ = 0;
        }
    }

    bool ok() const { return size_ > 0; }
    int  size() const { return size_; }

    // Dark module at (x, y)? Out-of-range reads are light, so the quiet zone
    // needs no special casing when drawing.
    bool module(int x, int y) const {
        return code_ && code_->getModule(x, y);
    }

    // Module size that fits the symbol plus a `quiet`-module margin into a
    // box of `boxSize` pixels. Returns 0 when the code failed to build.
    float cellFor(float boxSize, int quiet = 4) const {
        if (!ok()) return 0.0f;
        return boxSize / (float)(size_ + quiet * 2);
    }

private:
    std::optional<qrcodegen::QrCode> code_;
    int size_ = 0;
};

}  // namespace tcx::qr
