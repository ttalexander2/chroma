using System;

namespace Chroma
{
    public class Component
    {
        public Entity Entity;
    }

    public class Entity
    {
        public ulong ID { get; private set; }


        protected Entity() { ID = 0; }
        internal Entity(ulong id) { ID = id; }
        ~Entity() { }

        public T CreateComponent<T>() where T : Component, new()
        {
            //CreateComponent_Native(ID, typeof(T));
            T component = new T();
            component.Entity = this;
            return component;
        }

        public bool HasComponent<T>() where T : Component, new()
        {
            //return HasComponent_Native(ID, typeof(T));
            return false;
        }

        public bool HasComponent(Type type)
        {
            // return HasComponent_Native(ID, type);
            return false;
        }

        public T GetComponent<T>() where T : Component, new()
        {
            if (HasComponent<T>())
            {
                T component = new T();
                component.Entity = this;
                return component;
            }
            return null;
        }

        public Entity FindEntityByTag(string tag)
        {
            return this;
        }


        
    }
}