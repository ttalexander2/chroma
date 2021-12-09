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

        internal static IEnumerable<Type> GetInheritanceHierarchy(Type type)
        {
            for (var current = type; current != typeof(Entity); current = current.BaseType)
                yield return current;
        }

        internal static string[] GetEntityTypeHierarchy()
        {
            List<string> _types = new List<string>();
            foreach (Assembly ass in AppDomain.CurrentDomain.GetAssemblies())
            {
                foreach (Type type in ass.GetTypes())
                {
                    if (type.IsSubclassOf(typeof(Entity)))
                    {
                        string path = "";
                        foreach (Type sub in GetInheritanceHierarchy(type))
                        {
                            if (path != "")
                                path = sub.Name + "." + path;
                            else
                                path = sub.Name;
                        }
                        path = type.Namespace + "." + path;
                        _types.Add(path);  
                    }
                }
            }
            return _types.ToArray();
        }
    }
}