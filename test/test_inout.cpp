#include "CppJack.h"

#include "unit_test_framework.h"

#include <vector>
#include <thread>

using namespace std;
using namespace jack;

class InOutSwitch : public Callback {
public:
    InOutSwitch() {
        modeIn = true;
        modeOut = false;
        playhead = 0;
    }

    void process(int n, vector<vector<sample_t>>& output, vector<vector<sample_t>>& input) override {
        if (modeIn) {
            rec.reserve(rec.size() + n);
            for (int i = 0; i < n; i++) {
                rec.push_back(input[0][i]);
            }
        }
        if (modeOut) {
            for (int i = 0; i < n; i++) {
                output[0][i] = rec[playhead];
                output[1][i] = rec[playhead];
                playhead++;
                if (playhead >= rec.size()) playhead = 0;
            }
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

    vector<sample_t> rec;
};

int main() {
    Client client(2, 1, "CppJack_test_inout");
    test::check(client.isOpen(), "Client not open");

    InOutSwitch sw;

    client.start(&sw);
    cout << "Recording...\n";

    this_thread::sleep_for(chrono::seconds(2));

    sw.switchMode();
    cout << "Playing...\n";

    this_thread::sleep_for(chrono::seconds(2));

    client.stop();
    client.close();
}
