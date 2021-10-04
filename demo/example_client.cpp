#include "CppJack.h"

#include <thread>
#include <iostream>
#include <algorithm>

/* Unix signal handling (part 1) */
#include <csignal>
#include <functional>
// Wrapper for the shutdown lambda expression
std::function<void(int)> shutdownHandler;
// Function given as the signal handler
void signalHandler(int signal) { shutdownHandler(signal); }
/* End part 1 */

using namespace jack;

// Takes one channel of input, halves the amplitude, and sends it to all output channels.
class SignalHalver : public Callback {
public:
    void process(int n, std::vector<std::vector<sample_t>>& output, std::vector<std::vector<sample_t>>& input) override {
        std::vector<sample_t> halved(n);

        for (auto& chan : input) {
            for (int i = 0; i < n; i++) {
                halved[i] = 0.5f * chan[i];
            }
        }

        for (auto& chan : output)
            std::copy(halved.begin(), halved.end(), chan.begin());
    }
};


int main(int argc, char* argv[]) {
    // The client
    jack::Client client;

    /* Unix signal handling (part 2) */
    // Connect signals to handler function
    signal(SIGINT, signalHandler);
    signal(SIGQUIT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGHUP, signalHandler);
    signal(SIGKILL, signalHandler);
    signal(SIGTSTP, signalHandler);
    // Define shutdown handler expression
    shutdownHandler = [&client](int signal) {
        std::cout << "CppJack example_client: caught signal " << signal << '\n';
        client.close();
    };
    /* End part 2 */

    // The callback used to process samples
    SignalHalver halver;

    // Open the client
    client.open(2, 1, "CppJack_example_client");

    // Start DSP
    client.start(&halver);

    // Run the program until the user presses a key
    std::cout << "Press Enter to stop the demo.\n";
    getchar();

    // Stop DSP
    client.stop();

    // Close the client
    client.close();

    return 0;
}