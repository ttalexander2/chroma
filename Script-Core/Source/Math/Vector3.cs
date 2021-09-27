using System;
using System.Runtime.InteropServices;

namespace Chroma
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector3 : IEquatable<Vector3>
    {
        public float X;
        public float Y;
        public float Z;

        public static Vector3 Zero = new Vector3(0, 0, 0);
        public static Vector3 One = new Vector3(1, 1, 1);
        public static Vector3 Forward = new Vector3(0, 0, -1);
        public static Vector3 Right = new Vector3(1, 0, 0);
        public static Vector3 Up = new Vector3(0, 1, 0);
        public static Vector3 Down = new Vector3(0, -1, 0);


        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }

        public Vector3(float scalar)
        {
            X = Y = Z = scalar;
        }

        public Vector3(float x, float y)
        {
            X = x;
            Y = y;
            Z = 0;
        }

        public Vector3(Vector2 xy)
        {
            X = xy.X;
            Y = xy.Y;
            Z = 0;
        }

        public Vector3(Vector2 xy, float z)
        {
            X = xy.X;
            Y = xy.Y;
            Z = z;
        }

        public Vector3(Vector4 vec)
        {
            X = vec.X;
            Y = vec.Y;
            Z = vec.Z;
        }

        public void Clamp(Vector3 min, Vector3 max)
        {
            X = Mathf.Clamp(X, min.X, max.X);
            Y = Mathf.Clamp(Y, min.Y, max.Y);
            Z = Mathf.Clamp(Z, min.Z, max.Z);
        }

        public float Length()
        {
            return Mathf.Sqrt(X * X + Y * Y + Z * Z);
        }

        public Vector3 Normalized()
        {
            float len = Length();
            float x = X;
            float y = Y;
            float z = Z;

            if (len > 0)
            {
                x /= len;
                y /= len;
                z /= len;
            }

            return new Vector3(x, y, z);
        }

        public void Normalize()
        {
            float len = Length();
            if (len > 0)
            {
                X /= len;
                Y /= len;
                Z /= len;
            }
        }


        public float Distance(Vector3 other)
        {
            return Mathf.Sqrt(Mathf.Square(other.X - X) + Mathf.Square(other.Y - Y) + Mathf.Square(other.Z - Z));
        }

        public static float Distance(Vector3 first, Vector3 second)
        {
            return Mathf.Sqrt(Mathf.Square(second.X - first.X) + Mathf.Square(second.Y - first.Y) + Mathf.Square(second.Z - first.Z));
        }

        public static Vector3 Lerp(Vector3 from, Vector3 to, float delta)
        {
            if (delta < 0.0f)
                return from;
            if (delta > 1.0f)
                return to;
            return from + ((to - from) * delta);
        }

        public static Vector3 operator *(Vector3 left, float scalar)
        {
            return new Vector3(left.X * scalar, left.Y * scalar, left.Z * scalar);
        }

        public static Vector3 operator *(float scalar, Vector3 right)
        {
            return new Vector3(scalar * right.X, scalar * right.Y, scalar * right.Z);
        }

        public static Vector3 operator *(Vector3 left, Vector3 right)
        {
            return new Vector3(left.X * right.X, left.Y * right.Y, left.Z * right.Z);
        }

        public static Vector3 operator +(Vector3 left, Vector3 right)
        {
            return new Vector3(left.X + right.X, left.Y + right.Y, left.Z + right.Z);
        }

        public static Vector3 operator +(Vector3 left, float right)
        {
            return new Vector3(left.X + right, left.Y + right, left.Z + right);
        }

        public static Vector3 operator -(Vector3 left, Vector3 right)
        {
            return new Vector3(left.X - right.X, left.Y - right.Y, left.Z - right.Z);
        }

        public static Vector3 operator /(Vector3 left, Vector3 right)
        {
            return new Vector3(left.X / right.X, left.Y / right.Y, left.Z / right.Z);
        }

        public static Vector3 operator /(Vector3 left, float scalar)
        {
            return new Vector3(left.X / scalar, left.Y / scalar, left.Z / scalar);
        }

        public static Vector3 operator -(Vector3 vector)
        {
            return new Vector3(-vector.X, -vector.Y, -vector.Z);
        }

        public override bool Equals(object obj) => obj is Vector3 other && this.Equals(other);

        public bool Equals(Vector3 right) => X == right.X && Y == right.Y && Z == right.Z;

        public static bool operator ==(Vector3 left, Vector3 right) => left.Equals(right);

        public static bool operator !=(Vector3 left, Vector3 right) => !(left == right);

        public static Vector3 Cos(Vector3 vector)
        {
            return new Vector3(Mathf.Cos(vector.X), Mathf.Cos(vector.Y), Mathf.Cos(vector.Z));
        }

        public static Vector3 Sin(Vector3 vector)
        {
            return new Vector3(Mathf.Sin(vector.X), Mathf.Sin(vector.Y), Mathf.Sin(vector.Z));
        }

        public override string ToString()
        {
            return "[" + X + ", " + Y + ", " + Z + "]";
        }

        public override int GetHashCode()
        {
            int hashCode = -307843816;
            hashCode = hashCode * -1521134295 + X.GetHashCode();
            hashCode = hashCode * -1521134295 + Y.GetHashCode();
            hashCode = hashCode * -1521134295 + Z.GetHashCode();
            return hashCode;
        }

        public Vector2 XY
        {
            get { return new Vector2(X, Y); }
            set { X = value.X; Y = value.Y; }
        }
        public Vector2 XZ
        {
            get { return new Vector2(X, Z); }
            set { X = value.X; Z = value.Y; }
        }
        public Vector2 YZ
        {
            get { return new Vector2(Y, Z); }
            set { Y = value.X; Z = value.Y; }
        }



    }
}
