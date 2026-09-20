// =============================================================================
// example-qr - QR code generation demo
// =============================================================================

#pragma once

#include <TrussC.h>
#include "tcQr.h"

#include <cstdio>
#include <memory>
#include <string>

using namespace tc;

class tcApp : public App {
public:
    void setup() override;
    void draw() override;

    void keyPressed(int key) override;

private:
    void rebuild();   // re-encode after text_ or ecc_ changed

    std::string text_ = "https://github.com/TrussC-org/TrussC";
    tcx::qr::Ecc ecc_ = tcx::qr::Ecc::Medium;

    // Code has no default constructor (it encodes in its ctor), so hold it
    // by pointer and swap in a fresh one whenever the input changes.
    std::unique_ptr<tcx::qr::Code> code_;
};
