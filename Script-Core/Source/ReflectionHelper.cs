using System;
using System.Reflection;
using System.Collections.Generic;


namespace Chroma
{
    internal class ReflectionHelper
    {

        internal static string[] GetEntityTypes()
        {
            List<string> _types = new List<string>();
            foreach (Assembly ass in AppDomain.CurrentDomain.GetAssemblies())
            {
                foreach (Type type in ass.GetTypes())
                {
                    if (type.IsSubclassOf(typeof(Entity)))
                    {
                        _types.Add(type.FullName);
                    }
                }
            }
            return _types.ToArray();
        }
    }
}