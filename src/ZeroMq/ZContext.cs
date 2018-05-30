﻿using System;
using System.Net.Sockets;
using System.Text;

using ZeroMQ.lib;

namespace ZeroMQ
{
    /// <summary>
    /// Creates <see cref="ZSocket"/> instances within a process boundary.
    /// </summary>
    public sealed class ZContext : MemoryCheck
    {
#if UNMANAGE_MONEY_CHECK
        protected override string TypeName => nameof(ZContext);
#endif
        /// <summary>
		/// Gets and protected sets the default Encoding.
		/// Note: Do not set the Encoding after ZContext.Create.
		/// </summary>
		public static Encoding Encoding { get; } = Encoding.UTF8;

        private static readonly object SyncObject = new object();
        /// <summary>
        /// 初始化
        /// </summary>
	    public static void Initialize()
        {
            lock (SyncObject)
            {
                if (_current == null)
                {
                    _current = new ZContext();
                }
            }
        }

        /// <summary>
        ///     关闭
        /// </summary>
        public static void Destroy()
        {
            _current?.Dispose();
        }

        private static ZContext _current;
        /// <summary>
        /// 当前默认实例
        /// </summary>
		public static ZContext Current
        {
            get
            {
                if (_current == null)
                {
                    // INFO: This is the ZContext who is the one, running this program.
                    lock (SyncObject)
                    {
                        if (_current == null)
                        {
                            _current = new ZContext();
                        }
                    }
                }
                return _current;
            }
        }

        /// <summary>
        /// Create a <see cref="ZContext"/> instance.
        /// </summary>
        /// <returns><see cref="ZContext"/></returns>
        public ZContext()
        {
            _contextPtr = zmq.ctx_new();

            if (_contextPtr == IntPtr.Zero)
            {
                throw new InvalidProgramException("zmq_ctx_new");
            }
        }

        /// <summary>
        /// Create a <see cref="ZContext"/> instance.
        /// </summary>
        /// <returns><see cref="ZContext"/></returns>
        public static ZContext Create()
        {
            return new ZContext();
        }

        private IntPtr _contextPtr;

        /// <summary>
        /// Gets a handle to the native ZeroMQ context.
        /// </summary>
        public IntPtr ContextPtr => _contextPtr;

        public static bool Has(string capability)
        {
            using (var capabilityPtr = DispoIntPtr.AllocString(capability))
            {
                if (0 < zmq.has(capabilityPtr))
                {
                    return true;
                }
            }
            return false;
        }

        public static void Proxy(ZSocket frontend, ZSocket backend)
        {
            Proxy(frontend, backend, null);
        }

        public static bool Proxy(ZSocket frontend, ZSocket backend, out ZError error)
        {
            return Proxy(frontend, backend, null, out error);
        }

        public static void Proxy(ZSocket frontend, ZSocket backend, ZSocket capture)
        {
            ZError error;
            if (!Proxy(frontend, backend, capture, out error))
            {
                if (error == ZError.ETERM)
                {
                    return; // Interrupted
                }
                throw new ZException(error);
            }
        }

        public static bool Proxy(ZSocket frontend, ZSocket backend, ZSocket capture, out ZError error)
        {
            error = ZError.None;

            while (-1 == zmq.proxy(frontend.SocketPtr, backend.SocketPtr, capture == null ? IntPtr.Zero : capture.SocketPtr))
            {
                error = ZError.GetLastErr();

                if (error == ZError.EINTR)
                {
                    error = default(ZError);
                    continue;
                }
                return false;
            }
            return true;
        }

        public static void ProxySteerable(ZSocket frontend, ZSocket backend, ZSocket control)
        {
            ProxySteerable(frontend, backend, null, control);
        }

        public static bool ProxySteerable(ZSocket frontend, ZSocket backend, ZSocket control, out ZError error)
        {
            return ProxySteerable(frontend, backend, null, control, out error);
        }

        public static void ProxySteerable(ZSocket frontend, ZSocket backend, ZSocket capture, ZSocket control)
        {
            ZError error;
            if (!ProxySteerable(frontend, backend, capture, control, out error))
            {
                if (error == ZError.ETERM)
                {
                    return; // Interrupted
                }
                throw new ZException(error);
            }
        }

        public static bool ProxySteerable(ZSocket frontend, ZSocket backend, ZSocket capture, ZSocket control, out ZError error)
        {
            error = ZError.None;

            while (-1 == zmq.proxy_steerable(frontend.SocketPtr, backend.SocketPtr, capture == null ? IntPtr.Zero : capture.SocketPtr, control == null ? IntPtr.Zero : control.SocketPtr))
            {
                error = ZError.GetLastErr();

                if (error == ZError.EINTR)
                {
                    error = default(ZError);
                    continue;
                }
                return false;
            }
            return true;
        }

        public void SetOption(ZContextOption option, int optionValue)
        {
            EnsureNotDisposed();

            int rc = zmq.ctx_set(_contextPtr, (Int32)option, optionValue);
            if (rc != -1) return;
            var error = ZError.GetLastErr();

            if (Equals(error, ZError.EINVAL))
            {
                throw new ArgumentOutOfRangeException(
                    $"The requested option optionName \"{option}\" is invalid.");
            }
            throw new ZException(error);
        }

        public int GetOption(ZContextOption option)
        {
            EnsureNotDisposed();

            int rc = zmq.ctx_get(_contextPtr, (Int32)option);
            if (rc != -1) return rc;
            var error = ZError.GetLastErr();

            if (Equals(error, ZError.EINVAL))
            {
                throw new ArgumentOutOfRangeException(
                    $"The requested option optionName \"{option}\" is invalid.");
            }
            throw new ZException(error);
        }

        /// <summary>
        /// Gets or sets the size of the thread pool for the current context (default = 1).
        /// </summary>
        public int ThreadPoolSize
        {
            get => GetOption(ZContextOption.IO_THREADS);
            set => SetOption(ZContextOption.IO_THREADS, value);
        }

        /// <summary>
        /// Gets or sets the maximum number of sockets for the current context (default = 1024).
        /// </summary>
        public int MaxSockets
        {
            get => GetOption(ZContextOption.MAX_SOCKETS);
            set => SetOption(ZContextOption.MAX_SOCKETS, value);
        }

        /// <summary>
        /// Gets or sets the supported socket protocol(s) when using TCP transports. (Default = <see cref="ProtocolType.Ipv4Only"/>).
        /// </summary>
        public bool IPv6Enabled
        {
            get => GetOption(ZContextOption.IPV6) == 1;
            set => SetOption(ZContextOption.IPV6, value ? 1 : 0);
        }

        /// <summary>
        /// Shutdown the ZeroMQ context.
        /// </summary>
        public void Shutdown()
        {
            ZError error;
            if (!Shutdown(out error))
            {
                throw new ZException(error);
            }
        }

        /// <summary>
        /// Shutdown the ZeroMQ context.
        /// </summary>
        public bool Shutdown(out ZError error)
        {
            error = default(ZError);

            if (_contextPtr == IntPtr.Zero)
                return true;

            while (-1 == zmq.ctx_shutdown(_contextPtr))
            {
                error = ZError.GetLastErr();
                if (!Equals(error, ZError.EINTR))
                    return false;
                error = default(ZError);
            }

            // don't _contextPtr = IntPtr.Zero;
            return true;
        }

        /// <inheritdoc />
        protected override void DoDispose()
        {
            Terminate(out var error);
        }

        /// <summary>
        /// Terminate the ZeroMQ context.
        /// </summary>
        public bool Terminate(out ZError error)
        {
            error = ZError.None;
            if (_contextPtr == IntPtr.Zero)
                return true;

            while (-1 == zmq.ctx_term(_contextPtr))
            {
                error = ZError.GetLastErr();

                if (error == ZError.EINTR)
                {
                    error = default(ZError);
                    continue;
                }

                // Maybe ZError.EFAULT

                return false;
            }

            _contextPtr = IntPtr.Zero;
            return true;
        }

        /// <summary>
        /// Terminate the ZeroMQ context.
        /// </summary>
        public void Terminate()
        {
            ZError error;
            if (!Terminate(out error))
            {
                throw new ZException(error);
            }
        }

        private void EnsureNotDisposed()
        {
            if (_contextPtr == IntPtr.Zero)
            {
                throw new ObjectDisposedException(GetType().FullName);
            }
        }
    }
}