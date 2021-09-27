using System;
using System.Runtime.InteropServices;

namespace Chroma
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector4 : IEquatable<Vector4>
    {
        public float X;
        public float Y;
        public float Z;
        public float W;

        public static Vector4 Zero = new Vector4(0, 0, 0, 0);
        public static Vector4 One = new Vector4(1, 1, 1, 1);

        public Vector4(float x, float y, float z, float w)
        {
            X = x;
            Y = y;
            Z = z;
            W = w;
        }

        public Vector4(float scalar)
        {
            X = Y = Z = W = scalar;
        }

        public Vector4(Vector3 vec)
        {
            X = vec.X;
            Y = vec.Y;
            Z = vec.Z;
            W = 0;
        }

        public Vector4(Vector3 vec, float w)
        {
            X = vec.X;
            Y = vec.Y;
            Z = vec.Z;
            W = w;
        }

        public Vector4(Vector2 vec)
        {
            X = vec.X;
            Y = vec.Y;
            Z = 0;
            W = 0;
        }

        public Vector4(Vector2 vec, float z, float w)
        {
            X = vec.X;
            Y = vec.Y;
            Z = z;
            W = w;
        }

        public void Clamp(Vector4 min, Vector4 max)
        {
            X = Mathf.Clamp(X, min.X, max.X);
            Y = Mathf.Clamp(Y, min.Y, max.Y);
            Z = Mathf.Clamp(Z, min.Z, max.Z);
            W = Mathf.Clamp(W, min.W, max.W);
        }

        public float Length()
        {
            return Mathf.Sqrt(X * X + Y * Y + Z * Z + W * W);
        }

        public Vector4 Normalized()
        {
            float len = Length();
            float x = X;
            float y = Y;
            float z = Z;
            float w = W;

            if (len > 0)
            {
                x /= len;
                y /= len;
                z /= len;
                w /= len;
            }

            return new Vector4(x, y, z, w);
        }

        public void Normalize()
        {
            float len = Length();
            if (len > 0)
            {
                X /= len;
                Y /= len;
                Z /= len;
                W /= len;
            }
        }

        public float Distance(Vector4 other)
        {
            return Mathf.Sqrt(Mathf.Square(other.X - X) + Mathf.Square(other.Y - Y) + Mathf.Square(other.Z - Z) + Mathf.Square(other.W - W));
        }

        public static float Distance(Vector4 first, Vector4 second)
        {
            return Mathf.Sqrt(Mathf.Square(second.X - first.X) + Mathf.Square(second.Y - first.Y) + Mathf.Square(second.Z - first.Z) + Mathf.Square(second.W - first.W));
        }

        public static Vector4 Lerp(Vector4 from, Vector4 to, float t)
        {
            if (t < 0.0f) t = 0.0f;
            if (t > 1.0f) t = 1.0f;
            return (1.0f - t) * from + t * to;

        }

        public static Vector4 operator *(Vector4 left, float scalar)
        {
            return new Vector4(left.X * scalar, left.Y * scalar, left.Z * scalar, left.W * scalar);
        }

        public static Vector4 operator *(float scalar, Vector4 right)
        {
            return new Vector4(scalar * right.X, scalar * right.Y, scalar * right.Z, right.W * scalar);
        }

        public static Vector4 operator *(Vector4 left, Vector4 right)
        {
            return new Vector4(left.X * right.X, left.Y * right.Y, left.Z * right.Z, left.W * right.W);
        }

        public static Vector4 operator +(Vector4 left, Vector4 right)
        {
            return new Vector4(left.X + right.X, left.Y + right.Y, left.Z + right.Z, left.W + right.W);
        }

        public static Vector4 operator +(Vector4 left, float right)
        {
            return new Vector4(left.X + right, left.Y + right, left.Z + right, left.W + right);
        }

        public static Vector4 operator -(Vector4 left, Vector4 right)
        {
            return new Vector4(left.X - right.X, left.Y - right.Y, left.Z - right.Z, left.W - right.W);
        }

        public static Vector4 operator /(Vector4 left, Vector4 right)
        {
            return new Vector4(left.X / right.X, left.Y / right.Y, left.Z / right.Z, left.W / right.W);
        }

        public static Vector4 operator /(Vector4 left, float scalar)
        {
            return new Vector4(left.X / scalar, left.Y / scalar, left.Z / scalar, left.W / scalar);
        }

        public static Vector4 operator -(Vector4 vector)
        {
            return new Vector4(-vector.X, -vector.Y, -vector.Z, -vector.W);
        }

        public override bool Equals(object obj) => obj is Vector4 other && this.Equals(other);

        public bool Equals(Vector4 right) => X == right.X && Y == right.Y && Z == right.Z && W == right.W;

        public static bool operator ==(Vector4 left, Vector4 right) => left.Equals(right);

        public static bool operator !=(Vector4 left, Vector4 right) => !(left == right);

        public static Vector4 Cos(Vector4 vector)
        {
            return new Vector4(Mathf.Cos(vector.X), Mathf.Cos(vector.Y), Mathf.Cos(vector.Z), Mathf.Cos(vector.W));
        }

        public static Vector4 Sin(Vector4 vector)
        {
            return new Vector4(Mathf.Sin(vector.X), Mathf.Sin(vector.Y), Mathf.Sin(vector.Z), Mathf.Cos(vector.W));
        }

        public override string ToString()
        {
            return "[" + X + ", " + Y + ", " + Z + ", " + W + "]";
        }

        public override int GetHashCode()
        {
            int hashCode = 707706286;
            hashCode = hashCode * -1521134295 + X.GetHashCode();
            hashCode = hashCode * -1521134295 + Y.GetHashCode();
            hashCode = hashCode * -1521134295 + Z.GetHashCode();
            hashCode = hashCode * -1521134295 + W.GetHashCode();
            return hashCode;
        }
    }
}
