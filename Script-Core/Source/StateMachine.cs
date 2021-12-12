using System;
using System.Collections.Generic;

namespace Chroma
{

    public class StateMachine<T> where T : struct, IConvertible
    {
        public T Current { get; private set; }
        private Dictionary<T, Action> _update = new Dictionary<T, Action>();
        private Dictionary<T, Coroutine> _coroutines = new Dictionary<T, Coroutine>();
        private Dictionary<T, Action> _begin = new Dictionary<T, Action>();
        private Dictionary<T, Action> _end = new Dictionary<T, Action>();

        public StateMachine(T initial_state)
        {
            if (!typeof(T).IsEnum)
            {
                throw new ArgumentException("T must be an enumerated type");
            }

            Current = initial_state;
        }

        public void Add(T state, Action update, Coroutine.CoroutineFunction coroutine = null, Action begin = null, Action end = null)
        {
            if (_update.ContainsKey(state))
            {
                throw new ArgumentException("State already exists in StateMachine");
            }

            if (_update != null)
                _update[state] = update;
            if (begin != null)
                _begin[state] = begin;
            if (end != null)
                _end[state] = end;
            if (coroutine != null)
                _coroutines[state] = new Coroutine(coroutine);
        }

        public void Set(T state)
        {
            if (_coroutines.ContainsKey(Current))
                _coroutines[Current].Stop();
            if (_end.ContainsKey(Current))
                _end[Current].Invoke();
            Current = state;
            if (_begin.ContainsKey(Current))
                _begin[Current].Invoke();
            if (_coroutines.ContainsKey(Current))
                _coroutines[Current].Start();
        }

        public void Update()
        {
            if (_update.ContainsKey(Current) && _update[Current] != null)
                _update[Current].Invoke();
            if (_coroutines.ContainsKey(Current) && _coroutines[Current] != null)
                _coroutines[Current].Update();
        }





    }


    
}