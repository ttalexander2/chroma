using System;


namespace Chroma
{
    /// <summary>
    /// Float based math class for use in engine.
    /// </summary>
    public class Mathf
    {
        /// <summary>
        /// PI to a precision of 16 decimal digits.
        /// </summary>
        public const float PI = 3.1415926535897931f;

        /// <summary>
        /// E to a precision of 16 decimal digits.
        /// </summary>
        public const float E = 2.7182818284590451f;

        public static float Abs(float val)
        {
            return (float)System.Math.Abs((double)val);
        }

        public static float Acos(float val)
        {
            return (float)System.Math.Acos((double)val);
        }

        public static float Atan(float val)
        {
            return (float)System.Math.Atan((double)val);
        }

        public static float Atan2(float x, float y)
        {
            return (float)System.Math.Atan2((double)x, (double)y);
        }

        public static float Ceiling(float val)
        {
            return (float)System.Math.Ceiling((double)val);
        }

        public static float Cos(float val)
        {
            return (float)System.Math.Cos((double)val);
        }

        public static float Cosh(float val)
        {
            return (float)System.Math.Cosh((double)val);
        }


        public static float Clamp(float val, float min, float max)
        {
            if (val < min)
                return min;
            if (val > max)
                return max;
            return val;
        }

        public static T Clamp<T>(T val, T min, T max) where T : IComparable<T>
        {
            if (val.CompareTo(min) < 0)
                return min;
            if (val.CompareTo(max) > 0)
                return max;
            return val;
        }

        public static float Exp(float val)
        {
            return (float)System.Math.Exp(val);
        }

        public static float Floor(float val)
        {
            return (float)System.Math.Floor((double)val);
        }

        public static float IEEERemainder(float a, float b)
        {
            return (float)System.Math.IEEERemainder((double)a, (double)b);
        }

        public static float Log(float val)
        {
            return (float)System.Math.Log((double)val);
        }

        public static float Log(float val, float new_base)
        {
            return (float)System.Math.Log((double)val, (double)new_base);
        }

        public static float Log10(float val)
        {
            return (float)System.Math.Log10((double)val);
        }

        public static byte Min(byte a, byte b)
        {
            return System.Math.Min(a, b);
        }

        public static decimal Min(decimal a, decimal b)
        {
            return System.Math.Min(a, b);
        }

        public static double Min(double a, double b)
        {
            return System.Math.Min(a, b);
        }

        public static Int16 Min(Int16 a, Int16 b)
        {
            return System.Math.Min(a, b);
        }

        public static Int32 Min(Int32 a, Int32 b)
        {
            return System.Math.Min(a, b);
        }

        public static Int64 Min(Int64 a, Int64 b)
        {
            return System.Math.Min(a, b);
        }

        public static sbyte Min(sbyte a, sbyte b)
        {
            return System.Math.Min(a, b);
        }

        public static float Min(float a, float b)
        {
            return System.Math.Min(a, b);
        }

        public static UInt16 Min(UInt16 a, UInt16 b)
        {
            return System.Math.Min(a, b);
        }

        public static UInt32 Min(UInt32 a, UInt32 b)
        {
            return System.Math.Min(a, b);
        }

        public static UInt64 Min(UInt64 a, UInt64 b)
        {
            return System.Math.Min(a, b);
        }

        public static byte Max(byte a, byte b)
        {
            return System.Math.Max(a, b);
        }

        public static decimal Max(decimal a, decimal b)
        {
            return System.Math.Max(a, b);
        }

        public static double Max(double a, double b)
        {
            return System.Math.Max(a, b);
        }

        public static Int16 Max(Int16 a, Int16 b)
        {
            return System.Math.Max(a, b);
        }

        public static Int32 Max(Int32 a, Int32 b)
        {
            return System.Math.Max(a, b);
        }

        public static Int64 Max(Int64 a, Int64 b)
        {
            return System.Math.Max(a, b);
        }

        public static sbyte Max(sbyte a, sbyte b)
        {
            return System.Math.Max(a, b);
        }

        public static float Max(float a, float b)
        {
            return System.Math.Max(a, b);
        }

        public static UInt16 Max(UInt16 a, UInt16 b)
        {
            return System.Math.Max(a, b);
        }

        public static UInt32 Max(UInt32 a, UInt32 b)
        {
            return System.Math.Max(a, b);
        }

        public static UInt64 Max(UInt64 a, UInt64 b)
        {
            return System.Math.Max(a, b);
        }

        public static float Pow(float a, float b)
        {
            return (float)System.Math.Pow((double)a, (double)b);
        }

        public static float Round(float val)
        {
            return (float)System.Math.Round((double)val);
        }

        public static float Round(float val, Int32 rounding)
        {
            return (float)System.Math.Round((double)val, rounding);
        }

        public static int Sign(decimal val)
        {
            return System.Math.Sign(val);
        }

        public static int Sign(double val)
        {
            return System.Math.Sign(val);
        }

        public static int Sign(Int16 val)
        {
            return System.Math.Sign(val);
        }

        public static int Sign(Int32 val)
        {
            return System.Math.Sign(val);
        }

        public static int Sign(Int64 val)
        {
            return System.Math.Sign(val);
        }

        public static int Sign(SByte val)
        {
            return System.Math.Sign(val);
        }

        public static int Sign(float val)
        {
            return System.Math.Sign(val);
        }

        public static float Sin(float val)
        {
            return (float)System.Math.Sin((double)val);
        }

        public static float Sinh(float val)
        {
            return (float)System.Math.Sinh((double)val);
        }

        public static float Sqrt(float val)
        {
            return (float)System.Math.Sqrt((double)val);
        }

        public static float Square(float val)
        {
            return (float)System.Math.Pow((double)val, 2.0);
        }

        public static float Tan(float val)
        {
            return (float)System.Math.Tan((double)val);
        }

        public static float Tanh(float val)
        {
            return (float)System.Math.Tanh((double)val);
        }

        public static float Truncate(float val)
        {
            return (float)System.Math.Truncate((double)val);
        }


        public static float RadiansToDegrees(float radians)
        {
            return (180f / Mathf.PI) * radians;
        }

        public static float DegreesToRadians(float degrees)
        {
            return (Mathf.PI / 180f) * degrees;
        }
    }
}
