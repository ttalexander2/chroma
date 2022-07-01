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
        /// <summary>
        /// Numeric ID for the entity.
        /// </summary>
        public ulong ID { get; private set; }
        /// <summary>
        /// Name of the entity.
        /// </summary>
        public string Name => GetComponent<Tag>().Name;


        protected Entity() { ID = 0; }
        internal Entity(ulong id) { ID = id; }
        ~Entity() { }

        private List<Coroutine> _coroutines = new List<Coroutine>();

        /// <summary>
        /// Creates a coroutine and attaches it to this entity.
        /// </summary>
        /// <returns></returns>
        public Coroutine CreateCoroutine()
        {
            Coroutine c = new Coroutine();
            _coroutines.Add(c);
            return c;
        }

        /// <summary>
        /// Creates a coroutine, sets the function provided, and attaches it to this entity.
        /// </summary>
        /// <param name="function">Function for the coroutine to execute.</param>
        /// <returns></returns>
        public Coroutine CreateCoroutine(Coroutine.CoroutineFunction function)
        {
            Coroutine c = new Coroutine(function);
            _coroutines.Add(c);
            return c;
        }

        /// <summary>
        /// Creates a component of the given type and attaches it to this entity.
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <returns></returns>
        public T CreateComponent<T>() where T : Component, new()
        {
            //TODO: Should check if a component of this type exists first, if it does, either throw an exception or return the existing component?
            CreateComponent_Native(ID, typeof(T));
            T component = new T();
            component.Entity = this;
            return component;
        }

        /// <summary>
        /// Checks whether this entity has a component of a given type.
        /// </summary>
        /// <typeparam name="T">Type of component.</typeparam>
        /// <returns></returns>
        public bool HasComponent<T>() where T : Component, new()
        {
            return HasComponent_Native(ID, typeof(T));
        }

        /// <summary>
        /// Checks whether this entity has a component of a given type.
        /// </summary>
        /// <param name="type">Type of component.</param>
        /// <returns></returns>
        public bool HasComponent(Type type)
        {
            return HasComponent_Native(ID, type);
        }

        /// <summary>
        /// Gets this entity's component of a specific type.
        /// </summary>
        /// <typeparam name="T">Type of component.</typeparam>
        /// <returns>Returns component of type <c>T</c>, or <c>null</c> if this entity doesn't have a component of type T.</returns>
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
        
        /// <summary>
        /// Gets an entity its name. Returns <c>null</c> if an entity with the given name does not exist.
        /// </summary>
        /// <param name="name"></param>
        /// <returns></returns>
        public static Entity FindEntityByName(string name)
        {
            return new Entity(FindEntityByName_Native(name));
        }

        /// <summary>
        /// Gets a list of this entities' children. 
        /// </summary>
        /// <returns></returns>
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

        /// <summary>
        /// Gets a child of this entity by its name. Returns <c>null</c> if no such child exists.
        /// </summary>
        /// <param name="name"></param>
        /// <returns></returns>
        public Entity GetChildByName(string name)
        {
            ulong id = GetChildByName_Native(ID, name);
            return new Entity(id);
        }

        /// <summary>
        /// Gets the first child of this entity. Returns <c>null</c> if no children exist.
        /// </summary>
        /// <returns></returns>
        public Entity GetFirstChild()
        {
            ulong id = GetFirstChild_Native(ID);
            return new Entity(id);
        }

        /// <summary>
        /// Checks whether the given entity has children.
        /// </summary>
        /// <returns></returns>
        public bool HasChildren()
        {
            return HasChildren_Native(ID);
        }

        /// <summary>
        /// Gets the number of children for this entity.
        /// </summary>
        /// <returns></returns>
        public uint NumberChildren()
        {
            return NumChildren_Native(ID);
        }

        /// <summary>
        /// Calculates the absolute position of an entity. This applies if this entity has a parent.
        /// </summary>
        /// <returns></returns>
        public Vector2 GetAbsolutePosition()
        {
            Vector2 pos = new Vector2();
            GetAbsolutePosition_Native(ID, out pos);
            return pos;
        }

        /// <summary>
        /// Sets an absolute position of an entity. 
        /// If this entity has a parent, this will set this entity's transform relative to the parent's transform.
        /// </summary>
        /// <param name="position"></param>
        public void SetAbsolutePosition(Vector2 position)
        {
            SetAbsolutePosition_Native(ID, position);
        }

        /// <summary>
        /// Pre-initialization function. Called before init, after the scene is loaded.
        /// </summary>
        public virtual void PreInit() { }

        /// <summary>
        /// Initialization function. Called directly after <c cref="PreInit">PreInit()</c>.
        /// </summary>
        public virtual void Init() { }

        /// <summary>
        /// Post-initialization function. Called directly after <c cref="Init">Init()</c>.
        /// </summary>
        public virtual void PostInit() { }


        /// <summary>
        /// Early-Update function. Called at a rate of 60FPS.
        /// </summary>
        public virtual void EarlyUpdate() { }

        /// <summary>
        /// Update function. Called at a rate of 60FPS, directly after <c cref="EarlyUpdate">EarlyUpdate()</c>.
        /// </summary>
        public virtual void Update() { }

        /// <summary>
        /// Late-Update function. Called at a rate of 60FPS, directly after <c cref="Update">Update()</c>.
        /// </summary>
        public virtual void LateUpdate() { }

        /// <summary>
        /// Early-Update function. Called once per frame.
        /// </summary>
        public virtual void EarlyDraw() { }

        /// <summary>
        /// Update function. Called once per frame, directly after <c cref="EarlyDraw">EarlyDraw()</c>.
        /// </summary>
        public virtual void Draw() { }

        /// <summary>
        /// Late-Update function. Called once per frame, directly after <c cref="Draw">Draw()</c>.
        /// </summary>
        public virtual void LateDraw() { }

        /// <summary>
        /// Function called when this entity collides with another. Called every frame an overlap takes place.
        /// </summary>
        /// <param name="collisionEntity">The entity this object is colliding with.</param>
        public virtual void OnCollide(Entity collisionEntity) { }

        internal void Internal_OnCollide(ulong id) 
        {
            OnCollide(new Entity(id));
        }

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