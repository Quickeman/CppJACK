#ifndef CPPJACK_EXCEPTIONS_H
#define CPPJACK_EXCEPTIONS_H

#include <exception>
#include <string>

namespace jack {

class ClientException : public std::exception {
public:
    ClientException(std::string msg): msg(msg) {}

    const char* what() const throw() {
        return msg.data();
    }

private:
    std::string msg;
};

} // namespace jack

#endif
