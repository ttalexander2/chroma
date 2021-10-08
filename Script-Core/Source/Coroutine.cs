using System;
using System.Collections;
using System.Collections.Generic;

namespace Chroma
{
    public class Coroutine
    {
        public bool Active { get; private set; }
        public bool Finished { get; private set; }

        public delegate IEnumerator CoroutineFunction();

        private CoroutineFunction _func;
        private Stack<IEnumerator> _enumerators = new Stack<IEnumerator>();

        private float _waitTimer = 0;
        private bool _ended = false;

        internal Coroutine()
        {
            Active = false;
            Finished = false;
        }

        internal Coroutine(CoroutineFunction function)
        {
            Active = false;
            Finished = false;
            _func = function;
        }

        internal void Update()
        {
            _ended = false;
            if (_waitTimer > 0)
            {
                _waitTimer -= Time.DeltaF;
            }
            else if (_enumerators.Count > 0)
            {
                IEnumerator now = _enumerators.Peek();
                if (now.MoveNext() && !_ended)
                {
                    if (now.Current is int)
                    {
                        _waitTimer = (int)now.Current;
                    }
                    else if (now.Current is float || now.Current is double)
                    {
                        _waitTimer = (float)now.Current;
                    }
                    else if (now.Current is IEnumerator)
                        _enumerators.Push(now.Current as IEnumerator);
                }
                else if (!_ended)
                {
                    _enumerators.Pop();
                    if (_enumerators.Count == 0)
                    {
                        Finished = true;
                    }
                }
            }

        }

        public void Start()
        {
            if (Active)
            {
                Log.WriteLine("Coroutine already started!");
                return;
            }
            Active = true;
            Finished = false;
            _waitTimer = 0;
            _ended = false;
            _enumerators.Clear();
            _enumerators.Push(_func());
        }

        public void Pause()
        {
            Active = false;
        }

        public void Resume()
        {
            Active = true;
        }

        public void Restart()
        {
            Active = true;
            Finished = false;
            _waitTimer = 0;
            _ended = false;
            _enumerators.Clear();
            _enumerators.Push(_func());
        }

        public void Stop()
        {
            Active = false;
            Finished = true;
            _waitTimer = 0;
            _enumerators.Clear();

            _ended = true;
        }

        public void Set(CoroutineFunction function)
        {
            Finished = false;
            _waitTimer = 0;
            _enumerators.Clear();
            _func = function;

            _ended = false;
        }

    }
}
