#include "CppJack.h"

#include <thread>
#include <iostream>

#include <signal.h>

using namespace jack;

/* Unix signal handling (part 1) */
#include <functional>
// Wrapper for the shutdown lambda expression
std::function<void(int)> shutdownHandler;
// Function given as the signal handler
void signalHandler(int signal) { shutdownHandler(signal); }
/* End part 1 */


// Takes one channel of input, halves the amplitude, and sends it to all output channels.
class SignalHalver : public Callback {
public:
    void process(int n, std::vector<std::vector<sample_t>>& output, std::vector<std::vector<sample_t>>& input) override {
        std::vector<sample_t> halved(n);

        std::transform(input[0].begin(), input[0].end(), halved.begin(),
            [](sample_t& s) -> sample_t { return 0.5f * s; });

        std::for_each(output.begin(), output.end(), [&halved](std::vector<sample_t>& ch)
            { std::copy(halved.begin(), halved.end(), ch.begin()); });
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
    shutdownHandler = [&](int signal) {
        std::cout << "CppJack example_client: caught signal " << signal << "\n";
        client.close();
    };
    /* End part 2 */

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

    return 0;
}