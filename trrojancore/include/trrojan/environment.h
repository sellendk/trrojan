/// <copyright file="environment.h" company="SFB-TRR 161 Quantitative Methods for Visual Computing">
/// Copyright � 2016 SFB-TRR 161. Alle Rechte vorbehalten.
/// </copyright>
/// <author>Christoph M�ller</author>

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "trrojan/cmd_line.h"
#include "trrojan/device.h"
#include "trrojan/export.h"


namespace trrojan {

    /* Forward declarations. */
    class configuration;


    /// <summary>
    /// This class defines the interface for and some default implementation of
    /// an execution environment.
    /// </summary>
    /// <remarks>
    /// Execution environments provide the basic setup for some tests, eg by
    /// setting up OpenCL or other APIs.
    /// </remarks>
    class TRROJANCORE_API environment_base {

    public:

        /// <summary>
        /// A list of <see cref="trrojan::device" />s.
        /// </summary>
        typedef std::vector<device> device_list;

        /// <summary>
        /// Finalises the instance.
        /// </summary>
        virtual ~environment_base(void);

        /// <summary>
        /// Answer the (compute) devices the environment supports.
        /// </summary>
        /// <param name="dst">A <see cref="device_list" /> to append the
        /// devices to.</param>
        /// <returns>The number of devices which have been appended to
        /// <paramref name="dst" />.</returns>
        virtual size_t get_devices(device_list& dst) = 0;

        /// <summary>
        /// Gets the name of the execution environment.
        /// </summary>
        /// <returns>The name of the execution environment.</returns>
        inline const std::string& name(void) const {
            return this->_name;
        }

        /// <summary>
        /// The <see cref="trrojan::executive" /> calls this method before the
        /// environment is actually used to perform some benchmarks.
        /// </summary>
        /// <remarks>
        /// <para>The default implementation does nothing.</para>
        /// <para>This method should be used to allocate (graphics) resources.
        /// Failure should be indicated by throwing an exception.</para>
        /// </remarks>
        virtual void on_activate(void);

        /// <summary>
        /// The <see cref="trrojan::executive" /> calls this method to undo
        /// effects from <see cref="on_activate" />.
        /// </summary>
        /// <remarks>
        /// <para>The default implementation does nothing.</para>
        /// <para>This method should not throw!</para>
        /// </remarks>
        virtual void on_deactivate(void) noexcept;

        /// <summary>
        /// The <see cref="trrojan::executive" /> calls this method to undo
        /// effects from <see cref="on_initialise" />.
        /// </summary>
        /// <remarks>
        /// <para>The default implementation does nothing.</para>
        /// <para>The environment will not be used any more after this method
        /// has been called. All resources should be freed in the call.</para>
        /// <para>This method should not throw!</para>
        /// </remarks>
        virtual void on_finalise(void) noexcept;

        /// <summary>
        /// The <see cref="trrojan::executive" /> calls this method for one-time
        /// initialisation before the environment is actually used.
        /// </summary>
        /// <remarks>
        /// <para>The default implementation does nothing.</para>
        /// <para>The environment should use this method to perform
        /// initialisation tasks which might fail. Failure should be indicated
        /// by throwing an exception. If possible, (graphics) resources should
        /// be allocated on activation to prevent unused stuff being allocated
        /// while other environments being used.</para>
        /// </remarks>
        /// <param name="cmdLine"> The command line of the application
        /// invocation.</param>
        virtual void on_initialise(const cmd_line& cmdLine);

#ifdef TRROJANCORE_WITH_SPELLING_ERRORS
        inline void on_initialize(const cmd_line& cmdLine) {
            this->on_initialise(cmdLine);
        }
#endif /* TRROJANCORE_WITH_SPELLING_ERRORS */

        /// <summary>
        /// Convert a device name to an actual device if a device factor is in
        /// the configuration.
        /// </summary>
        /// <remarks>
        /// <para>The implementation searches <paramref name="config" /> for a
        /// string factor with the name specified as <param name="name" /> and
        /// adds, if not already present, a new factor called
        /// <param name="device" /> with the actual device returned by the
        /// <paramref name="get_devices" /> having the given name. If no such
        /// device is found, an exception is thrown.</para>
        /// <para>Subclasses might want to override this method to provide a
        /// custom and/or more efficient implementation.</para>
        /// <remarks>
        /// <param name="config">The configuration to be processed.</param>
        /// <param name="name">The name of the factor holding the device
        /// name. This defaults to &quot;device;&quot.</param>
        virtual void translate_device(configuration& config,
            const std::string& name = "device_name",
            const std::string& device = "device");

    protected:

        /// <summary>
        /// Initialises a new instance.
        /// </summary>
        /// <param name="name">The name of the environment. Note that this name
        /// must be globally unique.</param>
        inline environment_base(const std::string& name) : _name(name) { }

    private:

        std::string _name;
    };

    /// <summary>
    /// An execution environment.
    /// </summary>
    typedef std::shared_ptr<environment_base> environment;
}
