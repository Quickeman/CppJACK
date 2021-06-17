#include "CppJack.h"
#include <thread>

// Takes one channel of input, halves the amplitude, and sends it to all output channels.
class SignalHalver : public jack::Callback {
public:
    void process(int n, vector<jack::sample_t*> output, vector<jack::sample_t*> input) override {
        for (int c = 0; c < output.size(); c++) { // For each output channel...
            for (int i = 0; i < n; i++) { // For each frame/sample...
                output[c][i] = 0.5f * input[0][i];
            }
        }
    }
};

int main(int argc, char* argv[]) {
    // The client
    jack::Client client;
    // The callback used to process samples
    SignalHalver halver;

    // Open the client
    client.open(2, 1, "CppJack_example_client");

    // Start DSP
    client.start(&halver);

    // Let it run for a little bit
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Stop DSP
    client.stop();

    // Close the client
    client.close();
}