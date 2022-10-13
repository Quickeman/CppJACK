#include "CppJack.h"

#include "unit_test_framework.h"

#include <vector>
#include <thread>
#include <algorithm>

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
            for (int i = 0; i < n; i++) {
                sample_t samp = 0.f;
                for (auto& inCh : input) {
                    samp += inCh[i];
                }
                if (input.size() != 0)
                    samp = samp / float(input.size());
                rec.push_back(samp);
            }
        }

        if (modeOut) {
            auto endPoint = rec.begin() + playhead + n;
            if ((playhead + n) >= rec.size())
                endPoint = rec.end();
            for (auto& channel : output)
                std::copy(rec.begin() + playhead, endPoint, channel.begin());
            playhead += n;
            if (playhead >= rec.size())
                playhead = 0;
        }
    }

    void switchMode() {
        if (modeOut)
            rec.clear();
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
