//
// Copyright (c) ZeroC, Inc. All rights reserved.
//

using Ice.Instrumentation;

using System.Collections.Generic;

namespace IceInternal
{
    internal static class ObserverHelper
    {
        internal static IInvocationObserver? GetInvocationObserver(Ice.IObjectPrx proxy, string op,
                                                                   IReadOnlyDictionary<string, string> context)
        {
            if (proxy.Communicator.Observer is ICommunicatorObserver communicatorObserver)
            {
                IInvocationObserver? observer = communicatorObserver.GetInvocationObserver(proxy, op, context);
                observer?.Attach();
                return observer;
            }
            return null;
        }
    }
}
