using System;
using System.Globalization;
using System.Text.RegularExpressions;
using System.Reflection;

namespace Chroma.Engine
{
    internal static class ScriptCreationHelper
    {
        internal static bool ValidateIdentifier(string value)
        {
            return Microsoft.CSharp.CSharpCodeProvider.CreateProvider("C#").IsValidIdentifier(value);
        }

        internal static bool ValidateClassName(string value)
        {
            foreach (Assembly ass in AppDomain.CurrentDomain.GetAssemblies())
            {
                Type t = ass.GetType(value);
                if (t != null)
                    return false;
            }
            return true;
        }
    }

}