#include "framework/unit_test_framework.h"
#include "CppJack.h"

#include <math.h>
#include <vector>
#include <thread>

using namespace std;
using namespace jack;

class SineGenerator : public Callback {
public:
    SineGenerator(const float sample_rate): sRate(sample_rate) {
        norm = frequency / sRate;
        phase = 0.f;
    }

    void process(int n, vector<vector<sample_t>>& output, vector<vector<sample_t>>& input) override {
        vector<sample_t> sine(n);
        // Generate sine wave
        for (int i = 0; i < n; i++) {
            sine[i] = 0.5f * sinf(2.f * M_PI * phase);
            phase += norm;
            if (phase >= 1.f) 
                phase -= 1.f;
        }

        // Send to output(s)
        for (int c = 0; c < output.size(); c++) {
            output[c] = sine;
        }
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

    this_thread::sleep_for(chrono::seconds(2));

    client.stop();
    client.close();
}
