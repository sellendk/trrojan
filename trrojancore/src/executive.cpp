/// <copyright file="executive.cpp" company="Visualisierungsinstitut der Universit�t Stuttgart">
/// Copyright � 2016 - 2018 Visualisierungsinstitut der Universit�t Stuttgart. Alle Rechte vorbehalten.
/// Licensed under the MIT licence. See LICENCE.txt file in the project root for full licence information.
/// </copyright>
/// <author>Christoph M�ller</author>

#include "trrojan/executive.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>

#ifndef _WIN32
#include <dlfcn.h>
#endif /* !_WIN32 */

#include "trrojan/io.h"
#include "trrojan/log.h"
#include "trrojan/text.h"

#include "scripting_host.h"


/*
 * trrojan::executive::~executive
 */
trrojan::executive::~executive(void) {
    if (this->cur_environment != nullptr) {
        this->cur_environment->on_deactivate();
        this->cur_environment.reset();
    }

    for (auto e : this->environments) {
        if (e.second != nullptr) {
            // Finalise only valid environments. The "any" environment is
            // actually a nullptr.
            e.second->on_finalise();
        }
    }
    this->environments.clear();
}


/*
 * trrojan::executive::find_plugin
 */
trrojan::plugin trrojan::executive::find_plugin(const std::string& name) {
    auto it = std::find_if(this->plugins.begin(), this->plugins.end(),
        [&name](const plugin p) { return p->name() == name; });
    if (it != this->plugins.end()) {
        return *it;
    } else {
        log::instance().write(log_level::warning, "The plugin named \"{}\" "
            "does not exist or was not loaded.\n", name.c_str());
        return nullptr;
    }
}


/*
 * trrojan::executive::javascript
 */
void trrojan::executive::javascript(const std::string& path,
        output_base& output, const cool_down& coolDown) {
    scripting_host host(output, coolDown);
    host.run_script(*this, path);
}


/*
 * trrojan::executive::load_plugins
 */
void trrojan::executive::load_plugins(const cmd_line& cmdLine) {
    try {
        std::vector<std::string> paths;

        log::instance().write_line(log_level::verbose, "Clearing old "
            "environments and adding the \"none\" environment before loading "
            "plugins ...");
        this->environments.clear();
        this->environments[environment_base::none_name]
            = trrojan::environment();

        log::instance().write(log_level::verbose, "Considering plugins "
            "from the current working directory.\n");
        get_file_system_entries(std::back_inserter(paths), std::string("."),
            false, trrojan::has_extension(plugin_dll::extension));

#ifdef _WIN32
#ifdef _UWP
            std::array<wchar_t, MAX_PATH>  mfn;
            uint32_t path_size = MAX_PATH;
            DWORD actual_size{};

            actual_size = ::GetModuleFileName(NULL, mfn.data(), path_size);

            {
                auto it = std::find(mfn.rbegin(), mfn.rend(),
                    directory_separator_char);
                auto p = std::wstring(mfn.begin(), it.base()-1);
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
                std::string np = converter.to_bytes(p);

                log::instance().write(log_level::verbose, "Considering plugins "
                    "from the directory \"{}\" holding the executable.\n",
                    np.c_str());
                get_file_system_entries(std::back_inserter(paths), np,
                    false, trrojan::has_extension(plugin_dll::extension));
            }
#else
        {
            std::vector<char> mfn(MAX_PATH);
            if (::GetModuleFileName(NULL, mfn.data(),
                    static_cast<DWORD>(mfn.size()))) {
                auto it = std::find(mfn.rbegin(), mfn.rend(),
                    directory_separator_char);
                auto p = std::string(mfn.begin(), it.base());
                log::instance().write(log_level::verbose, "Considering plugins "
                    "from the directory \"{}\" holding the executable.\n",
                    p.c_str());
                get_file_system_entries(std::back_inserter(paths), p,
                    false, trrojan::has_extension(plugin_dll::extension));
            }
        }
#endif // _UWP
#endif /* _WIN32 */

        log::instance().write(log_level::verbose, "Found {} potential "
            "plugin(s).\n", paths.size());

        for (auto& path : paths) {
            try {
                auto dll = plugin_dll::open(path);
                auto ep = dll.find_entry_point();
                if (ep == nullptr) {
                    log::instance().write_line(log_level::warning, "Plugin "
                        "entry point was not found in \"{}\".", path.c_str());
                    continue;
                }

                log::instance().write(log_level::verbose, "Found a plugin "
                    "entry point in \"{}\".\n", path.c_str());
                auto plugin = trrojan::plugin(ep());
                if (plugin != nullptr) {
                    log::instance().write(log_level::verbose, "Found plugin "
                        "\"{}\" in \"{}\".\n", plugin->name().c_str(),
                        path.c_str());
                    this->plugins.push_back(std::move(plugin));
                    this->plugin_dlls.push_back(std::move(dll));
                }
            } catch (std::exception& ex) {
                log::instance().write_line(ex);
            }
        }

        log::instance().write(log_level::verbose, "{} plugin(s) have been "
            "loaded. Retrieving execution environments from them ...\n", 
            this->plugins.size());

        for (auto p : this->plugins) {
            std::vector<environment> envs;
            p->create_environments(envs);

            for (auto e : envs) {
                // First, handle potential violations of the contract with the
                // plugin. If the plugin returns invalid stuff, just skip it.
                if (e == nullptr) {
                    log::instance().write(log_level::debug, "The plugin \"{}\" "
                        "returned a nullptr as environment.\n",
                        p->name().c_str());
                    continue;
                }

                auto name = e->name();
                if (this->environments.find(name) != this->environments.end()) {
                    log::instance().write(log_level::debug, "The plugin \"{}\" "
                        "returned the environment \"{}\", which conflicts with "
                        "an already loaded environment. The new environment "
                        "will be ignored.\n", p->name().c_str(), name.c_str());
                    continue;
                }

                // Secon, initialise the plugin and add it to the map.
                try {
                    e->on_initialise(cmdLine);
                    this->environments.insert(std::make_pair(name, e));
                    log::instance().write(log_level::verbose, "The "
                        "environment \"{}\", provided by plugin \"{}\", was "
                        "successfully initialised.\n", name.c_str(),
                        p->name().c_str());

                } catch (std::exception& ex) {
                    log::instance().write_line(ex);
                    log::instance().write(log_level::verbose, "The "
                        "environment \"{}\", provided by plugin \"{}\", failed "
                        "to initialise. The environment will be ignored.\n",
                        name.c_str(), p->name().c_str());
                }
            }
        }

    } catch (std::exception& ex) {
        log::instance().write_line(ex);
    }
}

void trrojan::executive::add_plugin(const plugin& plugin, const cmd_line& cmdLine)
{
    try {
        
        this->plugins.push_back(std::move(plugin));

        std::vector<environment> envs;
        this->plugins.back()->create_environments(envs);

        for (auto e : envs) {
            // First, handle potential violations of the contract with the
            // plugin. If the plugin returns invalid stuff, just skip it.
            if (e == nullptr) {
                log::instance().write(log_level::debug, "The plugin \"{}\" "
                    "returned a nullptr as environment.\n",
                    this->plugins.back()->name().c_str());
                continue;
            }

            auto name = e->name();
            if (this->environments.find(name) != this->environments.end()) {
                log::instance().write(log_level::debug, "The plugin \"{}\" "
                    "returned the environment \"{}\", which conflicts with "
                    "an already loaded environment. The new environment "
                    "will be ignored.\n", this->plugins.back()->name().c_str(), name.c_str());
                continue;
            }

            // Secon, initialise the plugin and add it to the map.
            try {
                e->on_initialise(cmdLine);
                this->environments.insert(std::make_pair(name, e));
                log::instance().write(log_level::verbose, "The "
                    "environment \"{}\", provided by plugin \"{}\", was "
                    "successfully initialised.\n", name.c_str(),
                    this->plugins.back()->name().c_str());

            }
            catch (std::exception& ex) {
                log::instance().write_line(ex);
                log::instance().write(log_level::verbose, "The "
                    "environment \"{}\", provided by plugin \"{}\", failed "
                    "to initialise. The environment will be ignored.\n",
                    name.c_str(), this->plugins.back()->name().c_str());
            }
        }

    }
    catch (std::exception& ex) {
        log::instance().write_line(ex);
    }
}


/*
 * trrojan::executive::run
 */
void trrojan::executive::run(benchmark_base& benchmark,
        configuration_set configs, output_base& output,
        const cool_down& coolDown) {
    // Note: This method is called from the scripting interface and possibly
    // from other places we do not yet know. Therefore, we do not optimise
    // the order of the parameters, but keep them as they have been passed
    // to the method.

    auto eds = this->prepare_env_devs(configs);
    for (auto e : eds) {
        this->enable_environment(e.environment);
        configs.replace_factor(factor::from_manifestations(
            environment_base::factor_name, e.environment));

        for (auto d : e.devices) {
            configs.replace_factor(factor::from_manifestations(
                device_base::factor_name, d));

            benchmark.run(configs, [&output](result&& r) {
                output << r;
                return true;
            }, coolDown);
        }
    }

#if 0
    {
        auto it = config.find(benchmark_base::factor_environment);
        if (it != config.end()) {
            auto env = this->find_environment(*it);
            if (env == nullptr) {
                std::stringstream msg;
                msg << "The environment \"" << it->value() << "\", which was "
                    "requested by the user-provided configuration, could not "
                    "be found.";
                throw std::runtime_error(msg.str());
            }

            this->enable_environment(env);
        }
    }

    {
        auto it = config.find(benchmark_base::factor_device);
        if ((it != config.end())
                && (it->value().type() != variant_type::device)) {
        }
    }
#endif
}


/*
 * trrojan::executive::run
 */
void trrojan::executive::run(const benchmark& benchmark,
        const configuration_set& configs, output_base& output,
        const cool_down& coolDown) {
    if (benchmark == nullptr) {
        throw std::runtime_error("The benchmark to run must not be nullptr.");
    }

    this->run(*benchmark, configs, output, coolDown);
}


/*
 * trrojan::executive::trroll
 */
void trrojan::executive::trroll(const std::string& path, output_base& output,
        const cool_down& coolDown) {
    typedef trroll_parser::benchmark_configs bcs;
    auto bcss = trroll_parser::parse(path);
    std::vector<benchmark> benchmarks;
    plugin curPlugin;

    // Make sure that the benchmark configurations are grouped. This will ensure
    // that we are not repeatedly retrieving benchmarks from the plugins when
    // switching them.
    std::stable_sort(bcss.begin(), bcss.end(), [](const bcs& l, const bcs& r) {
        return (l.benchmark.compare(r.benchmark) < 0);
    });
    std::stable_sort(bcss.begin(), bcss.end(), [](const bcs& l, const bcs& r) {
        return (l.plugin.compare(r.plugin) < 0);
    });

    for (auto& b : bcss) {
        // First, make sure to find the requested plugin and instantiate the
        // benchmarks requested from this plugin.
        if ((curPlugin == nullptr) || (curPlugin->name() != b.plugin)) {
            curPlugin = this->find_plugin(b.plugin);
            if (curPlugin != nullptr) {
                benchmarks.clear();
                curPlugin->create_benchmarks(benchmarks);

            } else {
                log::instance().write(log_level::warning, "The plugin \"{}\" "
                    "required for the benchmark \"{}\" does not exist or was "
                    "not loaded. The benchmark will be skipped.\n",
                    b.plugin.c_str(), b.benchmark.c_str());
            }
        }

        // If the required plugin was loaded, make sure that the requested
        // benchmarks exist in this plugin.
        if (curPlugin != nullptr) {
            auto it = std::find_if(benchmarks.begin(), benchmarks.end(),
                [&b](const benchmark m) { return m->name() == b.benchmark; });
            // HAZARD: IT IS INHERENTLY UNSAFE TO RUN DIFFERENT TYPES OF BENCHMARKS WITH DIFFERENT FACTORS FROM THE SAME FILE, BECAUSE THE CSV WILL BECOME BOGUS IN THIS CASE!
            // Think about either preventing this or emitting new headers.
            if (it != benchmarks.end() && (*it != nullptr)) {
                log::instance().write(log_level::information, "Running "
                    "benchmark \"{}\" from plugin \"{}\".\n",
                    b.benchmark.c_str(), b.plugin.c_str());

                (**it).optimise_order(b.configs);
                this->run(*it, b.configs, output, coolDown);

            } else {
                log::instance().write(log_level::warning, "No benchmark named "
                    "\"{}\" was found in plugin \"{}\". The benchmark will be "
                    "skipped.\n", b.benchmark.c_str(), b.plugin.c_str());
            }
        }
    } /* end for (auto b : bcss) */
}


/*
 * trrojan::executive::enable_environment
 */
void trrojan::executive::enable_environment(const std::string& name) {
    auto it = this->environments.find(name);
    if (it != this->environments.end()) {
        this->enable_environment(it->second);

    } else {
        std::stringstream msg;
        msg << "The environment \"" << name << "\" does not exist or has not "
            "been loaded." << std::ends;
        throw std::invalid_argument(msg.str());
    }
}


/*
 * trrojan::executive::enable_environment
 */
void trrojan::executive::enable_environment(environment env) {
    if ((this->cur_environment != nullptr) && (this->cur_environment != env)) {
        log::instance().write_line(log_level::information, "Disabling "
            "environment \"{}\" ...", this->cur_environment->name().c_str());
        this->cur_environment->on_deactivate();
        this->cur_environment = nullptr;
    }

    if (this->cur_environment == nullptr) {
        this->cur_environment = env;

        if (this->cur_environment != nullptr) {
            log::instance().write_line(log_level::information, "Enabling "
                "environment \"{}\" ...", env->name().c_str());
            this->cur_environment->on_activate();
        } else {
            log::instance().write_line(log_level::information, "Enabling "
                "empty environment ...");
        }
    }
}


/*
 * trrojan::executive::find_environment
 */
trrojan::environment trrojan::executive::find_environment(const variant& v) {
    auto retval = this->environments.end();

    if (v.type() == variant_type::string) {
        auto name = v.as<std::string>();
        retval = this->environments.find(name);
        if (retval == this->environments.end()) {
            log::instance().write(log_level::error, "The environment "
                "\"{}\" does not exist.\n", name.c_str());
        }

    } else if (v.type() == variant_type::wstring) {
        auto name = trrojan::to_utf8(v.as<std::wstring>());

        retval = this->environments.find(name);
        if (retval == this->environments.end()) {
            log::instance().write(log_level::error, "The environment "
                "\"{}\" does not exist.\n", name.c_str());
        }

    } else if (v.type() == variant_type::environment) {
        return v.as<environment>();

    } else {
        log::instance().write(log_level::warning, "The variant specifying "
            "an environment name is not a string and will therefore be "
            "ignored.\n");
    }

    return (retval != this->environments.end()) ? retval->second : nullptr;
}


/*
 * trrojan::executive::prepare_env_devs
 */
std::vector<trrojan::executive::env_dev_set>
trrojan::executive::prepare_env_devs(const configuration_set& cs,
        const std::string& factorEnv, const std::string& factorDev) {
    std::vector<device> devices;
    std::vector<environment> environments;
    std::vector<trrojan::executive::env_dev_set> retval;
    auto restrictions = cs.find_factor(factorEnv);

    /* Determine which environments to use. */
    if ((restrictions != nullptr) && (restrictions->size() > 0)) {
        // A restriction on environments was specified, translate each of
        // those into a real environment.
        environments.reserve(restrictions->size());

        for (size_t i = 0; i < restrictions->size(); ++i) {
            auto env = this->find_environment((*restrictions)[i]);
            if (env != nullptr) {
                environments.push_back(std::move(env));
            }
        }

    } else {
        // No environments are given, so use all we know.
        environments.reserve(this->environments.size());
        for (auto& e : this->environments) {
            environments.push_back(e.second);
        }
    }

    /* Determine which devices to use for each environment. */
    for (auto e : environments) {
        assert(devices.empty());
        restrictions = cs.find_factor(factorDev);

        if (e == nullptr) {
            // The default environment should have only a default device.
            devices.push_back(static_cast<device>(nullptr));

        } else if ((restrictions != nullptr) && (restrictions->size() > 0)) {
            // A restriction on devices was specified, translate each of
            // those into a real device.
            devices.reserve(restrictions->size());

            this->enable_environment(e);
            for (size_t i = 0; i < restrictions->size(); ++i) {
                auto dev = e->get_device((*restrictions)[i]);
                if (dev != nullptr) {
                    devices.push_back(std::move(dev));
                }
            }

        } else {
            // No environments are given, so use all the environment has or
            // at least one empty device to make sure that device-free
            // benchmarks can run.
            this->enable_environment(e);
            e->get_devices(devices);
            if (devices.empty()) {
                log::instance().write_line(log_level::verbose, "The "
                    "environment \"{}\" has no devices, so use an empty dummy.",
                    e->name().c_str());
                devices.push_back(static_cast<device>(nullptr));
            }
        }

        retval.emplace_back(std::move(e), std::move(devices));
    }

    return retval;
}


/*
 * trrojan::executive::plugin_dll::entry_point_name
 */
const std::string trrojan::executive::plugin_dll::entry_point_name
= "get_trrojan_plugin";


/*
 * trrojan::executive::plugin_dll::extension
 */
const std::string trrojan::executive::plugin_dll::extension
#ifdef _WIN32
= ".dll";
#else /* _WIN32 */
= ".so";
#endif /* _WIN32 */


/*
 * trrojan::executive::plugin_dll::invalid_handle
 */
trrojan::executive::plugin_dll::handle_type
trrojan::executive::plugin_dll::invalid_handle
#ifdef _WIN32
= NULL;
#else /* _WIN32 */
= nullptr;
#endif /* _WIN32 */


/*
 * trrojan::executive::plugin_dll::open
 */
trrojan::executive::plugin_dll trrojan::executive::plugin_dll::open(
        const std::string& path) {
    plugin_dll retval;

#ifdef _WIN32

#ifdef _UWP
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wpath = converter.from_bytes(path.c_str());
    retval.handle = ::LoadPackagedLibrary(wpath.c_str(),0);
    auto nec = ::GetLastError();
#else
    auto oldErrorMode = ::SetErrorMode(SEM_FAILCRITICALERRORS);
    retval.handle = ::LoadLibraryExA(path.c_str(), NULL, 0);
    auto nec = ::GetLastError();
    ::SetErrorMode(oldErrorMode);
#endif // _UWP

    if (retval.handle == plugin_dll::invalid_handle) {
        std::stringstream msg;
        msg << "Failed opening DLL \"" << path << "\"." << std::ends;
        std::error_code ec(nec, std::system_category());
        throw std::system_error(ec, msg.str());
    }
#else /* _WIN32 */
    retval.handle = ::dlopen(path.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if (retval.handle == plugin_dll::invalid_handle) {
        throw std::runtime_error(::dlerror());
    }

#endif /* _WIN32 */

    return std::move(retval);
}


/*
 * trrojan::executive::plugin_dll::~plugin_dll
 */
trrojan::executive::plugin_dll::~plugin_dll(void) {
    try {
        this->close();
    } catch (...) { }
}


/*
 * trrojan::executive::plugin_dll::close
 */
void trrojan::executive::plugin_dll::close(void) {
    if (this->handle != plugin_dll::invalid_handle) {
#ifdef _WIN32
        if (::FreeLibrary(this->handle) != TRUE) {
            std::error_code ec(::GetLastError(), std::system_category());
            throw std::system_error(ec, "Failed to close DLL.");
        }
#else /* _WIN32 */
        auto ec = ::dlclose(this->handle);
        if (ec != 0) {
            throw std::runtime_error(::dlerror());
        }
#endif /* _WIN32 */

        this->handle = plugin_dll::invalid_handle;
    }
}


/*
 * trrojan::executive::plugin_dll::find
 */
trrojan::executive::plugin_dll::proc_type trrojan::executive::plugin_dll::find(
        const std::string& name) {
#ifdef _WIN32
    return ::GetProcAddress(this->handle, name.c_str());
#else /* _WIN32 */
    return ::dlsym(this->handle, name.c_str());
#endif /* _WIN32 */
}


/*
 * trrojan::executive::plugin_dll::operator =
 */
trrojan::executive::plugin_dll& trrojan::executive::plugin_dll::operator =(
        plugin_dll&& rhs) {
    if (this != std::addressof(rhs)) {
        this->handle = rhs.handle;
        rhs.handle = plugin_dll::invalid_handle;
    }

    return *this;
}
