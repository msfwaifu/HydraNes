#pragma once

#include <Hydra/Kernel/Lib.h>
#include <Hydra/Kernel/Input.h>
#include <Hydra/Application/Application.h>

#include "Nes.h"
#include "Ui.h"

using namespace hydra;

struct Profiler {

    StaticBuffer<1024> eventBuffer;
    ArrayView<uint8> eventBlob;
    EventStream events;

    struct commands {
        enum Type {
            start, end, push, pop
        };

        // command based profiling ?
        struct Push {
            cstring name;
            Time start;

            static uint32           Type() { return push; }
        };

        struct Pop {
            Time end;

            static uint32           Type() { return pop; }
        };
    };

    Profiler() {
        eventBlob = { eventBuffer.begin(), eventBuffer.maxSize() };
        events._blob = &eventBlob;
    }

    void Start() {

    }

    void End() {

    }

    void Push( cstring name ) {
        commands::Push e = { name, time::Now() };
        events.AddEvent<commands::Push>( &e );
    }

    void Pop() {
        commands::Pop e = { time::Now() };
        events.AddEvent<commands::Pop>( &e );
    }
};

struct RenderSystemNes;
struct AudioSystemNes;

struct MainState : public application::State {

    StaticBuffer<1024>  windowEventBuffer;
    ArrayView<uint8>    windowEventBlob;

    Nes                 nes;
    NesUI               nesui;

    WindowMean<32>      fpsValues;

    Profiler            profiler[2];
    Profiler*           currentProfiler;
    Profiler*           previousProfiler;
    uint8               profilerIndex = 0;

    input::InputSystem* input;
    WindowSystem*       window;
    TimeSystem*         time;
    RenderSystemNes*    renderer;
    AudioSystemNes*     audio;

    bool                cpuTestMode;
    FileHandle          cpuTestFile;
    TimeStamp           sramSaveTimer;

    HINSTANCE           hInstance;

    enum SystemOrders {
        ord_time,
        ord_window,
        ord_inputupdate,
        ord_input,
        ord_audio,
        ord_renderer
    };

    enum SimulationType {
        type_step,
        type_continuous,
        type_stopped
    };

    SimulationType simulationType;

    MainState();

    void Init( application::InitContext& context ) override;
    bool Update( application::UpdateContext& context  ) override;
    void Render( application::RenderContext& context  ) override;
    void Terminate( application::TerminateContext& context ) override;
    void Resize( uint16 width, uint16 height ) override;

    // Cpu test based on nestest.nes rom and nestest.log.txt file. It does a simple string comparison to check CPU registers and expected values for each step.
    void LoadCpuTest();
    void ExecuteCpuTest();


};