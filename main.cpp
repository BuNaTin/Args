#include "args.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>

bool checkStoiConversion(const std::vector<std::string> &v) {
    try {
        for (auto &&i : v) {
            std::ignore = std::stoi(i);
        }
    } catch (...) {
        return false;
    }
    return true;
}

int main(int argc, char *argv[]) {
    enum ARGS {
        HELP,
        HELLO,
        ADD,
        SIZE,
    };
    const std::string args[SIZE] = {
            /* HELP */ "help",
            /* HELLO */ "hello",
            /* ADD */ "add",
    };

    utils::Args argument;
    utils::Param help(args[HELP], "h");
    help.handler([&argument]() {
            std::cout << argument.defaultHelp();
        })
            .check(utils::Param::empty())
            .description("print this message");

    using uPrm = utils::Param;
    argument.add(help)
            .add(uPrm(args[ADD], "a")
                         .check([](auto &&p_v) {
                             if (!p_v) return false;
                             return checkStoiConversion(*p_v);
                         })
                         .description("Add given integers"))
            .add(uPrm(args[HELLO], "ola")
                         .handler([]() {
                             std::cout << "Hello world!\n";
                         })
                         .description("print hello world message"));

    if (!argument.process(argc, argv)) {
        std::cout << argument.defaultHelp();
        return 1;
    }

    if (argument.has(args[ADD])) {
        std::vector<std::string> integers = *argument.get(args[ADD]);
        int ans = 0;
        std::for_each(integers.begin(),
                      integers.end(),
                      [&ans](const std::string &str) {
                          ans += std::stoi(str);
                      });
        std::cout << "Result = " << ans << '\n';
    }

    return 0;
}