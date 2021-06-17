#include "CppJack.h"

#include <math.h>
#include <vector>
#include <thread>

class SineGenerator : public jack::Callback {
public:
    SineGenerator(const float sampleRate) {
        sRate = sampleRate;
        norm = frequency / sRate;
        phase = 0.f;
    }

    void process(int n, vector<jack::sample_t*> output, vector<jack::sample_t*> input) override {
        vector<jack::sample_t> sine(n);
        // Generate sine wave
        for (auto v : sine) {
            v = sinf(2.f * M_PI * norm * phase);
            phase += norm;
            if (phase >= 1.f) phase -= 1.f;
        }

        // Send to output(s)
        for (auto ch : output) {
            for (int i = 0; i < n; i++) {
                ch[i] = sine[i];
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
    assert(&client);
    assert(client.isOpen());

    SineGenerator sinGen(client.sampleRate());
    assert(&sinGen);

    client.start(&sinGen);

    std::this_thread::sleep_for(std::chrono::seconds(2));

    client.stop();
    client.close();
}
