using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;


namespace Chroma
{

    [StructLayout(LayoutKind.Sequential)]
    public struct ManifoldPoint
    {
        public Vector2 LocalPoint { get; private set; }
        public float NormalImpulse { get; private set; }
        public float TangentImpulse { get; private set; }

        private uint _key;

        internal ManifoldPoint(Vector2 localPoint, float normalImpulse, float tangentImpulse, uint key)
        {
            LocalPoint = localPoint;
            NormalImpulse = normalImpulse;
            TangentImpulse = tangentImpulse;
            _key = key;
        }

        public override bool Equals(object obj)
        {
            if (this.GetType() != obj.GetType())
                return false;
            return _key == ((ManifoldPoint)obj)._key;
        }

        public override int GetHashCode()
        {
            return _key.GetHashCode();
        }
    }

    public class CollisionManifold
    {
        public enum ManifoldType
        {
            Circles = 0,
            FaceA = 1,
            FaceB = 2
        }

        public List<ManifoldPoint> Points { get; private set; }
        public Vector2 LocalNormal { get; private set; }
        public Vector2 LocalPoint { get; private set; }
        public ManifoldType Type { get; private set; }

        internal CollisionManifold(Vector2 localNormal, Vector2 localPoint, ManifoldType type)
        {
            LocalNormal = localNormal;
            LocalPoint = localPoint;
            Type = type;
        }
        
    }
}