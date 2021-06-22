#include "framework/unit_test_framework.h"
#include "CppJack.h"

#include <math.h>
#include <vector>
#include <thread>

using namespace test;

class SineGenerator : public jack::Callback {
public:
    SineGenerator(const float sample_rate): sRate(sample_rate) {
        norm = frequency / sRate;
        phase = 0.f;
    }

    void process(int n, vector<jack::sample_t*> output, vector<jack::sample_t*> input) override {
        vector<jack::sample_t> sine(n);
        // Generate sine wave
        for (int i = 0; i < sine.size(); i++) {
            phase += norm;
            if (phase >= 1.f) phase -= 1.f;
            sine[i] = 0.5f * sinf(2.f * M_PI * phase);
        }

        // Send to output(s)
        for (int c = 0; c < output.size(); c++) {
            for (int i = 0; i < n; i++) {
                output[c][i] = sine[i];
            }
        }
    }

private:
    float sRate;
    const float frequency = 440.f;
    float norm;
    float phase;
};

int main(int argc, char* argv[]) {
    jack::Client client(2, 0, "CppJack_test_output");
    check(client.isOpen(), "Jack client failed to open");

    SineGenerator sinGen(client.sampleRate());

    client.start(&sinGen);

    std::this_thread::sleep_for(std::chrono::seconds(2));

    client.stop();
    client.close();
}
