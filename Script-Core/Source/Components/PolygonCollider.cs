using System;
using System.Runtime.CompilerServices;
using System.Collections.Generic;


namespace Chroma
{


    public class PolygonCollider : Collider
    {

        /// <summary>
        /// Readonly list of vertices in the polygon.
        /// At the moment, polygon vertices cannot be changed programatically.
        /// </summary>
        public List<Vertex> Vertices
        {
            get
            {
                GetVertices_Native(Entity.ID, out List<Vertex> value);
                return value;
            }
        }

        public override ColliderType GetColliderType()
        {
            return ColliderType.Polygon;
        }


        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetVertices_Native(ulong id, out List<Vertex> val);

    }

}