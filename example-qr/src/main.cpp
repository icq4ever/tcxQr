#include "tcApp.h"

int main() {
    tc::WindowSettings settings;
    settings.setSize(900, 620);
    settings.setTitle("tcxQr Example");

    return TC_RUN_APP(tcApp, settings);
}
