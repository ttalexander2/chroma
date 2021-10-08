using System;
using System.Runtime.InteropServices;


namespace Chroma
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector2 : IEquatable<Vector2>
    {
        public float X;
        public float Y;

        public static Vector2 Zero = new Vector2(0, 0);
        public static Vector2 One = new Vector2(1, 1);
        public static Vector2 Up = new Vector2(0, 1);
        public static Vector2 Down = new Vector2(0, -1);
        public static Vector2 Left = new Vector2(-1, 0);
        public static Vector2 Right = new Vector2(1, 0);

        public Vector2(float x, float y)
        {
            X = x;
            Y = y;
        }

        public Vector2(float scalar)
        {
            X = Y = scalar;
        }

        public void Clamp(Vector2 min, Vector2 max)
        {
            X = Mathf.Clamp(X, min.X, max.X);
            Y = Mathf.Clamp(Y, min.Y, max.Y);
        }

        public float Length()
        {
            return Mathf.Sqrt(X * X + Y * Y);
        }

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

        public void Normalize()
        {
            float len = Length();
            if (len > 0)
            {
                X /= len;
                Y /= len;
            }
        }

        public float Distance(Vector2 other)
        {
            return Mathf.Sqrt(Mathf.Square(other.X - X) + Mathf.Square(other.Y - Y));
        }

        public static float Distance(Vector2 first, Vector2 second)
        {
            return Mathf.Sqrt(Mathf.Square(second.X - first.X) + Mathf.Square(second.Y - first.Y));
        }

        public static Vector2 Lerp(Vector2 from, Vector2 to, float delta)
        {
            if (delta < 0.0f)
                return from;
            if (delta > 1.0f)
                return to;
            return from + ((to - from) * delta);
        }

        public static Vector2 operator *(Vector2 left, float scalar)
        {
            return new Vector2(left.X * scalar, left.Y * scalar);
        }

        public static Vector2 operator *(float scalar, Vector2 right)
        {
            return new Vector2(scalar * right.X, scalar * right.Y);
        }

        public static Vector2 operator *(Vector2 left, Vector2 right)
        {
            return new Vector2(left.X * right.X, left.Y * right.Y);
        }

        public static Vector2 operator +(Vector2 left, Vector2 right)
        {
            return new Vector2(left.X + right.X, left.Y + right.Y);
        }

        public static Vector2 operator +(Vector2 left, float right)
        {
            return new Vector2(left.X + right, left.Y + right);
        }

        public static Vector2 operator -(Vector2 left, Vector2 right)
        {
            return new Vector2(left.X - right.X, left.Y - right.Y);
        }

        public static Vector2 operator /(Vector2 left, Vector2 right)
        {
            return new Vector2(left.X / right.X, left.Y / right.Y);
        }

        public static Vector2 operator /(Vector2 left, float scalar)
        {
            return new Vector2(left.X / scalar, left.Y / scalar);
        }

        public static Vector2 operator -(Vector2 vector)
        {
            return new Vector2(-vector.X, -vector.Y);
        }

        public override bool Equals(object obj) => obj is Vector2 other && this.Equals(other);

        public bool Equals(Vector2 right) => X == right.X && Y == right.Y;

        public static bool operator ==(Vector2 left, Vector2 right) => left.Equals(right);

        public static bool operator !=(Vector2 left, Vector2 right) => !(left == right);

        public static Vector2 Cos(Vector2 vector)
        {
            return new Vector2(Mathf.Cos(vector.X), Mathf.Cos(vector.Y));
        }

        public static Vector2 Sin(Vector2 vector)
        {
            return new Vector2(Mathf.Sin(vector.X), Mathf.Sin(vector.Y));
        }

        public override string ToString()
        {
            return "[" + X + ", " + Y + "]";
        }

        public override int GetHashCode()
        {
            int hashCode = 1861411795;
            hashCode = hashCode * -1521134295 + X.GetHashCode();
            hashCode = hashCode * -1521134295 + Y.GetHashCode();
            return hashCode;
        }
    }
}
