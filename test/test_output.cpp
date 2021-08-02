#include "framework/unit_test_framework.h"
#include "CppJack.h"

#include <math.h>
#include <vector>
#include <thread>
#include <algorithm>

using namespace jack;

class SineGenerator : public Callback {
public:
    SineGenerator(const float sample_rate): sRate(sample_rate) {
        norm = frequency / sRate;
        phase = 0.f;
    }

    void process(int n, std::vector<std::vector<sample_t>>& output, std::vector<std::vector<sample_t>>& input) override {
        std::vector<sample_t> sine(n);
        // Generate sine wave
        std::for_each(sine.begin(), sine.end(), [&](sample_t& s){
            s = 0.5f * sinf(2.f * M_PI * phase);
            phase += norm;
            if (phase >= 1.f) 
                phase -= 1.f;
        });

        // Send to output(s)
        std::for_each(output.begin(), output.end(),
            [&sine](std::vector<sample_t>& ch){ ch = sine; });
    }

private:
    float sRate;
    const float frequency = 440.f;
    float norm;
    float phase;
};

int main(int argc, char* argv[]) {
    Client client(2, 0, "CppJack_test_output");
    test::check(client.isOpen(), "Jack client failed to open");

    SineGenerator sinGen(client.sampleRate());

    client.start(&sinGen);

    std::this_thread::sleep_for(std::chrono::seconds(2));

    client.stop();
    client.close();
}
