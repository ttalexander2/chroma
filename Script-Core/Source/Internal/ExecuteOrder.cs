using System;
using System.Reflection;
using System.Collections;
using System.Collections.Generic;

namespace Chroma
{

    [System.AttributeUsage(System.AttributeTargets.Class | System.AttributeTargets.Struct)]
    public class ExecutionOrder : System.Attribute
    {
        public int Order { get; private set; }

        public ExecutionOrder(int order)
        {
            this.Order = order;
        }

    }

    static internal class ExecutionOrderHelper
    {
        static internal int GetOrderFromTypeName(string name)
        {
            Type type = null;
            foreach (Assembly ass in AppDomain.CurrentDomain.GetAssemblies())
            {
                Type t = ass.GetType(name);
                if (t != null)
                    type = t;
            }

            if (type == null)
                return 0;
            return GetOrderFromType(type);
        }

        static internal int GetOrderFromType(Type t)
        {
            if (t.IsSubclassOf(typeof(Entity)))
            {
                ExecutionOrder attribute = (ExecutionOrder)Attribute.GetCustomAttribute(t, typeof(ExecutionOrder));
                if (attribute != null)
                {
                    return attribute.Order;
                }
            }

            return 0;
        }
    }
}