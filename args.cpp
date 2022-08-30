#include "args.h"

#include <algorithm>
#include <iostream>

namespace {

bool isArg(char *word) { return word[0] == '-'; }

} // namespace

namespace utils {

Param::Param(const std::string &name, const std::string &short_name)
        : m_name(name), m_short(short_name) {
    ;
}

Param &&Param::handler(handler_t &&handl) {
    m_handler = std::move(handl);
    return std::move(*this);
}

Param &&Param::check(check_t &&checker) {
    m_check = std::move(checker);
    return std::move(*this);
}

Param &&Param::description(const std::string& str) {
    m_description = str;
    return std::move(*this);
}

bool Param::operator==(const std::string &name) {
    if (("--" + m_name) == name) {
        return true;
    }
    if (("-" + m_name) == name) {
        return true;
    }
    if (("--" + m_short) == name) {
        return true;
    }
    if (("-" + m_short) == name) {
        return true;
    }
    return false;
}

Param::check_t Param::empty() {
    return [](auto &&params) {
        return !params;
    };
}

Args &&Args::add(Param &&param) {
    m_data.push_back(std::move(param));
    return std::move(*this);
}

Args &&Args::add(const Param& param) {
    m_data.push_back(param);
    return std::move(*this);
}

Args &&Args::add(Param &&param, const std::string& description) {
    param.description(description);
    m_data.push_back(std::move(param));
    return std::move(*this);
}

bool Args::process(int argc, char *argv[]) {
    if (argc < 1) {
        return false;
    }

    // insertion part
    for (int i = 1; i < argc; ++i) {
        if (isArg(argv[i])) {
            // add arg
            auto it = std::find_if(m_data.begin(),
                                   m_data.end(),
                                   [name = std::string(argv[i])](
                                           auto &&param) -> bool {
                                       return param == name;
                                   });
            if (it == m_data.end()) {
                return false;
            }

            m_args.push_back(std::make_pair(*it, std::nullopt));
        } else {
            // add param to last arg
            std::size_t ind = m_args.size();
            if (ind == 0) {
                return false;
            }
            --ind;
            if (!m_args[ind].second) {
                m_args[ind].second = std::vector<std::string>();
            }
            (*m_args[ind].second).push_back(argv[i]);
        }
    }

    // validation part
    for (auto &&argument : m_args) {
        auto &&checker = argument.first.m_check;
        if (checker) {
            if (!(*checker)(argument.second)) {
                return false;
            }
        }
    }

    // handlers part
    for (auto &&argument : m_args) {
        auto &&handler = argument.first.m_handler;
        if (handler) (*handler)();
    }

    return true;
}

bool Args::has(const std::string &name) const noexcept {
    return std::find_if(m_args.begin(),
                        m_args.end(),
                        [&name](auto &&data) -> bool {
                            return name == data.first.m_name;
                        }) != m_args.end();
}

Param::Options Args::get(const std::string &name) const noexcept {
    auto it = std::find_if(
            m_args.begin(), m_args.end(), [&name](auto &&data) -> bool {
                return name == data.first.m_name;
            });

    if (it == m_args.end()) {
        return std::nullopt;
    }

    return it->second;
}

std::string Args::defaultHelp() const noexcept {
    std::string description = "Arguments: \n";
    for(auto&& arg : m_data) {
        description += " - ";
        description += arg.m_name;
        description += " [";
        description += arg.m_short;
        description += ']';

        if(arg.m_description) {
            description += " - ";
            description += (*arg.m_description);
        }
    
        description += '\n';
    }
    return description;
}

} // namespace utils
