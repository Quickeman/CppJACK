#include <exception>
#include <string>
#include <iostream>

namespace test {

class TestFailedExcept : public std::exception {
public:
    TestFailedExcept(std::string msg): msg(msg) {}

    const char* what() const throw() {
        return msg.data();
    }

private:
    std::string msg;
};

void check(bool test_cond, std::string fail_msg) {
    if (!test_cond) {
        try {
            throw TestFailedExcept(fail_msg);
        } 
        catch(const TestFailedExcept& e) {
            std::cerr << "Test failed: " << e.what() << "\n";
            exit(1);
        }
    }
}

} // namespace test
