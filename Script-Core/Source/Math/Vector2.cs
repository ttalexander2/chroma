using System;
using System.Runtime.InteropServices;


namespace Chroma
{
    /// <summary>
    /// 2-Component Vector Structure.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector2 : IEquatable<Vector2>
    {
        /// <summary>
        /// X component of the vector.
        /// </summary>
        public float X;
        /// <summary>
        /// Y component of the vector.
        /// </summary>
        public float Y;


        public static Vector2 Zero = new Vector2(0, 0);
        public static Vector2 One = new Vector2(1, 1);
        public static Vector2 Up = new Vector2(0, 1);
        public static Vector2 Down = new Vector2(0, -1);
        public static Vector2 Left = new Vector2(-1, 0);
        public static Vector2 Right = new Vector2(1, 0);

        /// <summary>
        /// Constructs a new vector with intial values.
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        public Vector2(float x, float y)
        {
            X = x;
            Y = y;
        }

        /// <summary>
        /// Constructs a new vector with an initial scalar (sets x and y to scalar).
        /// </summary>
        /// <param name="scalar"></param>
        public Vector2(float scalar)
        {
            X = Y = scalar;
        }

        /// <summary>
        /// Clamps the this vector between two vectors. The function performes a component-wise clamp, meaning clamping each component is done independently.
        /// </summary>
        /// <param name="min"></param>
        /// <param name="max"></param>
        public void Clamp(Vector2 min, Vector2 max)
        {
            X = Mathf.Clamp(X, min.X, max.X);
            Y = Mathf.Clamp(Y, min.Y, max.Y);
        }

        /// <summary>
        /// Calculates the length of the vector.
        /// </summary>
        /// <returns></returns>
        public float Length()
        {
            return Mathf.Sqrt(X * X + Y * Y);
        }

        /// <summary>
        /// Returns a normalized version of this vector. 
        /// Note: This does not normalize this vector, but rather returns a copy which is normalized.
        /// See <c cref="Normalize">Normalize</c> to normalize this vector.
        /// </summary>
        /// <returns></returns>
        public Vector2 Normalized()
        {
            float len = Length();
            float x = X;
            float y = Y;

            if (len > 0)
            {
                x /= len;
                y /= len;
            }

            return new Vector2(x, y);
        }

        /// <summary>
        /// Normalizes this vector.
        /// </summary>
        public void Normalize()
        {
            float len = Length();
            if (len > 0)
            {
                X /= len;
                Y /= len;
            }
        }

        /// <summary>
        /// Calculates the distance between this and another vector.
        /// </summary>
        /// <param name="other">Other vector to calculate distance.</param>
        /// <returns>Distance between this and other.</returns>
        public float Distance(Vector2 other)
        {
            return Mathf.Sqrt(Mathf.Square(other.X - X) + Mathf.Square(other.Y - Y));
        }

        /// <summary>
        /// Calculates the distance between two vectors.
        /// </summary>
        /// <param name="first"></param>
        /// <param name="second"></param>
        /// <returns>Distance between the two vectors.</returns>
        public static float Distance(Vector2 first, Vector2 second)
        {
            return Mathf.Sqrt(Mathf.Square(second.X - first.X) + Mathf.Square(second.Y - first.Y));
        }

        //TODO: Add non static lerp
        // Add angle (signed/unsigned) calculation functions

        /// <summary>
        /// Linear interpolation between two vectors.
        /// </summary>
        /// <param name="from">Starting vector</param>
        /// <param name="to">Ending vector</param>
        /// <param name="delta">Delta time</param>
        /// <returns></returns>
        public static Vector2 Lerp(Vector2 from, Vector2 to, float delta)
        {
            if (delta < 0.0f)
                return from;
            if (delta > 1.0f)
                return to;
            return from + ((to - from) * delta);
        }

        /// <summary>
        /// Calculates the angle between two vectors.
        /// </summary>
        /// <param name="a"></param>
        /// <param name="b"></param>
        /// <returns></returns>
        public static float Angle(Vector2 a, Vector2 b)
        {
            float angle = Mathf.Atan2(b.X, b.X) - Mathf.Atan2(a.Y, a.Y);
            return Mathf.Clamp(angle, 0, Mathf.PI * 2.0f);
        }

        /// <summary>
        /// Calculates the dot product between two vectors.
        /// </summary>
        /// <param name="a"></param>
        /// <param name="b"></param>
        /// <returns></returns>
        public static float Dot(Vector2 a, Vector2 b)
        {
            return a.X * b.X +
                   a.Y * b.Y;
        }

        /// <summary>
        /// Multiplication by scalar.
        /// </summary>
        /// <param name="left"></param>
        /// <param name="scalar"></param>
        /// <returns></returns>
        public static Vector2 operator *(Vector2 left, float scalar)
        {
            return new Vector2(left.X * scalar, left.Y * scalar);
        }

        /// <summary>
        /// Multipication by scalar.
        /// </summary>
        /// <param name="scalar"></param>
        /// <param name="right"></param>
        /// <returns></returns>
        public static Vector2 operator *(float scalar, Vector2 right)
        {
            return new Vector2(scalar * right.X, scalar * right.Y);
        }

        /// <summary>
        /// Multipication of two vectors.
        /// </summary>
        /// <param name="left"></param>
        /// <param name="right"></param>
        /// <returns></returns>
        public static Vector2 operator *(Vector2 left, Vector2 right)
        {
            return new Vector2(left.X * right.X, left.Y * right.Y);
        }

        /// <summary>
        /// Addition of two vectors.
        /// </summary>
        /// <param name="left"></param>
        /// <param name="right"></param>
        /// <returns></returns>
        public static Vector2 operator +(Vector2 left, Vector2 right)
        {
            return new Vector2(left.X + right.X, left.Y + right.Y);
        }

        /// <summary>
        /// Component-wise addition of a value to a vector.
        /// </summary>
        /// <param name="left"></param>
        /// <param name="right"></param>
        /// <returns></returns>
        public static Vector2 operator +(Vector2 left, float right)
        {
            return new Vector2(left.X + right, left.Y + right);
        }

        /// <summary>
        /// Subtraction of two vectors.
        /// </summary>
        /// <param name="left"></param>
        /// <param name="right"></param>
        /// <returns></returns>
        public static Vector2 operator -(Vector2 left, Vector2 right)
        {
            return new Vector2(left.X - right.X, left.Y - right.Y);
        }

        /// <summary>
        /// Division of two vectors.
        /// </summary>
        /// <param name="left"></param>
        /// <param name="right"></param>
        /// <returns></returns>
        public static Vector2 operator /(Vector2 left, Vector2 right)
        {
            return new Vector2(left.X / right.X, left.Y / right.Y);
        }

        /// <summary>
        /// Division of a vector by a scalar.
        /// </summary>
        /// <param name="left"></param>
        /// <param name="scalar"></param>
        /// <returns></returns>
        public static Vector2 operator /(Vector2 left, float scalar)
        {
            return new Vector2(left.X / scalar, left.Y / scalar);
        }

        /// <summary>
        /// Negation of a vector.
        /// </summary>
        /// <param name="vector"></param>
        /// <returns></returns>
        public static Vector2 operator -(Vector2 vector)
        {
            return new Vector2(-vector.X, -vector.Y);
        }

        /// <summary>
        /// Calculates equality between two vectors.
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        public override bool Equals(object obj) => obj is Vector2 other && this.Equals(other);

        /// <summary>
        /// Calculates equality between two vectors.
        /// </summary>
        /// <param name="right"></param>
        /// <returns></returns>
        public bool Equals(Vector2 right) => X == right.X && Y == right.Y;

        /// <summary>
        /// Calculates equality between two vectors.
        /// </summary>
        /// <param name="left"></param>
        /// <param name="right"></param>
        /// <returns></returns>
        public static bool operator ==(Vector2 left, Vector2 right) => left.Equals(right);

        /// <summary>
        /// Calculates non-equality between two vectors.
        /// </summary>
        /// <param name="left"></param>
        /// <param name="right"></param>
        /// <returns></returns>
        public static bool operator !=(Vector2 left, Vector2 right) => !(left == right);

        /// <summary>
        /// Computes the cosine of a vector, component-wise.
        /// </summary>
        /// <param name="vector"></param>
        /// <returns></returns>
        public static Vector2 Cos(Vector2 vector)
        {
            return new Vector2(Mathf.Cos(vector.X), Mathf.Cos(vector.Y));
        }

        /// <summary>
        /// Computes the sine of a vector, component-wise.
        /// </summary>
        /// <param name="vector"></param>
        /// <returns></returns>
        public static Vector2 Sin(Vector2 vector)
        {
            return new Vector2(Mathf.Sin(vector.X), Mathf.Sin(vector.Y));
        }

        /// <summary>
        /// Creates a formatted string containing the components of the vector.
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            return "[" + X + ", " + Y + "]";
        }

        /// <summary>
        /// Computes the hash code of the vector.
        /// </summary>
        /// <returns></returns>
        public override int GetHashCode()
        {
            int hashCode = 1861411795;
            hashCode = hashCode * -1521134295 + X.GetHashCode();
            hashCode = hashCode * -1521134295 + Y.GetHashCode();
            return hashCode;
        }
    }
}
