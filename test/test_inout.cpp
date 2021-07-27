#include "CppJack.h"

#include "unit_test_framework.h"

#include <vector>
#include <thread>

using namespace jack;

class InOutSwitch : public Callback {
public:
    InOutSwitch() {
        modeIn = true;
        modeOut = false;
        playhead = 0;
    }

    void process(int n, std::vector<std::vector<sample_t>>& output, std::vector<std::vector<sample_t>>& input) override {
        if (modeIn) {
            std::copy(input[0].begin(), input[0].end(), std::back_inserter(rec));
        }
        if (modeOut) {
            std::vector<sample_t>::iterator endPoint = \
                (playhead + n) > rec.size() ? rec.end() : rec.begin() + playhead + n;
            std::for_each(output.begin(), output.end(), [&](std::vector<sample_t>& ch)
                { std::copy(rec.begin()+playhead, endPoint, ch.begin()); });
            playhead += n;
            if (playhead >= rec.size()) playhead = 0;
        }
    }

    void switchMode() {
        if (modeOut) rec.clear();
        modeIn = !modeIn;
        modeOut = !modeOut;
        playhead = 0;
    }

private:
    bool modeIn;
    bool modeOut;
    int playhead;

    std::vector<sample_t> rec;
};

int main() {
    Client client(2, 1, "CppJack_test_inout");
    test::check(client.isOpen(), "Client not open");

    InOutSwitch sw;

    client.start(&sw);
    std::cout << "Recording...\n";

    std::this_thread::sleep_for(std::chrono::seconds(2));

    sw.switchMode();
    std::cout << "Playing...\n";

    std::this_thread::sleep_for(std::chrono::seconds(2));

    client.stop();
    client.close();
}
