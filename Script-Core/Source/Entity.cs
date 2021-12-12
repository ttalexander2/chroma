using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.CompilerServices;


namespace Chroma
{
    /// <summary>
    /// Main class for game entities.
    /// </summary>
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

        public Entity FindEntityByName(string name)
        {
            return new Entity(FindEntityByName_Native(name));
        }

        public List<Entity> GetChildren()
        {
            List<ulong> ids = GetChildren_Native(ID);
            List<Entity> entities = new List<Entity>();
            foreach (ulong id in ids)
            {
                entities.Add(new Entity(id));
            }
            return entities;
        }

        public Entity GetChildByName(string name)
        {
            ulong id = GetChildByName_Native(ID, name);
            return new Entity(id);
        }

        public Entity GetFirstChild()
        {
            ulong id = GetFirstChild_Native(ID);
            return new Entity(id);
        }

        public bool HasChildren()
        {
            return HasChildren_Native(ID);
        }

        public uint NumberChildren()
        {
            return NumChildren_Native(ID);
        }

        public Vector2 GetAbsolutePosition()
        {
            Vector2 pos = new Vector2();
            GetAbsolutePosition_Native(ID, out pos);
            return pos;
        }
        public void SetAbsolutePosition(Vector2 position)
        {
            SetAbsolutePosition_Native(ID, position);
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

        //This was public before so it might break something
        internal void InternalUpdate()
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

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern ulong FindEntityByName_Native(string name);



        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern List<ulong> GetChildren_Native(ulong entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern ulong GetChildByName_Native(ulong entityID, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern ulong GetFirstChild_Native(ulong entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool HasChildren_Native(ulong entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern uint NumChildren_Native(ulong entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetAbsolutePosition_Native(ulong entityID, out Vector2 result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetAbsolutePosition_Native(ulong entityID, Vector2 position);



    }
}