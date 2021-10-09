using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.CompilerServices;


namespace Chroma
{

    public class Entity
    {
        public ulong ID { get; private set; }
        public string Name => GetComponent<Tag>().Name;


        protected Entity() { ID = 0; }
        internal Entity(ulong id) { ID = id; }
        ~Entity() { }

        private List<Coroutine> _coroutines = new List<Coroutine>();

        public Coroutine CreateCoroutine()
        {
            Coroutine c = new Coroutine();
            _coroutines.Add(c);
            return c;
        }
        public Coroutine CreateCoroutine(Coroutine.CoroutineFunction function)
        {
            Coroutine c = new Coroutine(function);
            _coroutines.Add(c);
            return c;
        }

        public T CreateComponent<T>() where T : Component, new()
        {
            CreateComponent_Native(ID, typeof(T));
            T component = new T();
            component.Entity = this;
            return component;
        }

        public bool HasComponent<T>() where T : Component, new()
        {
            return HasComponent_Native(ID, typeof(T));
        }

        public bool HasComponent(Type type)
        {
            return HasComponent_Native(ID, type);
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

        public virtual void PreInit() { }
        public virtual void Init() { }
        public virtual void PostInit() { }
        public virtual void EarlyStart() { }
        public virtual void Start() { }
        public virtual void LateStart() { }
        public virtual void EarlyUpdate() { }
        public virtual void Update() { }
        public virtual void LateUpdate() { }

        public void InternalUpdate()
        {
           foreach (Coroutine c in _coroutines)
           {
                if (c.Active)
                    c.Update();
           }
        }



        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void CreateComponent_Native(ulong entityID, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool HasComponent_Native(ulong entityID, Type type);

    }
}