#include "xserver/audio_service.h"
#include "shared/protocol.h"
#include "shared/logger.h"
#include "xserver/crash_handler.h"
#include <csignal>
#include <unistd.h>
#include <atomic>

static std::atomic<bool> g_running{true};

static void signal_handler(int) {
    g_running = false;
}

int main() {
    fbx::shared::Logger::init("xaudio");
    fbx::xserver::installCrashHandler("xaudio");
    LOG_INFO("xaudio starting...");

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    fbx::xserver::AudioService& svc = fbx::xserver::AudioService::getInstance();
    if (!svc.init()) {
        LOG_WARN("AudioService init failed (no DSP hardware?). Running in degraded mode.");
    }

    LOG_INFO("AudioService running. Waiting for SIGTERM/SIGINT.");
    while (g_running) {
        // Audio mixing loop runs in AudioService's internal thread.
        // This main thread only needs to stay alive.
        pause();
    }

    LOG_INFO("xaudio shutting down.");
    svc.shutdown();
    return 0;
}
