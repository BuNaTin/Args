#pragma once

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace utils {

class Param {
    // interface
public:
    using Options = std::optional<std::vector<std::string>>;
    using handler_t = std::function<void()>;
    using check_t = std::function<bool(const Options &)>;

    static check_t empty();

public:
    Param(const std::string &name, const std::string &m_short);

    Param &&handler(handler_t &&handl);
    Param &&check(check_t &&checker);
    Param &&description(const std::string &str);

    bool operator==(const std::string &name);

    // constructors
public:
    Param() = delete;
    Param(const Param &other) = default;
    Param(Param &&other) = default;
    ~Param() = default;

    // data
public:
    std::string m_name;
    std::string m_short;
    std::optional<std::string> m_description;
    std::optional<handler_t> m_handler;
    std::optional<check_t> m_check;
};

class Args {
    // interface
public:
    Args &&add(Param &&param);
    Args &&add(const Param &param);
    Args &&add(Param &&param, const std::string &description);

    bool process(int argc, char *argv[]);

    bool has(const std::string &param_name) const noexcept;
    Param::Options get(const std::string &param_name) const noexcept;

    std::string defaultHelp() const noexcept;

    // data
private:
    std::vector<Param> m_data;

    using Args_t = std::pair<Param, Param::Options>;
    std::vector<Args_t> m_args;
};

// name: -ind [params]

} // namespace utils
