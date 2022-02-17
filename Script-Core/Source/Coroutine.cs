using System;
using System.Collections;
using System.Collections.Generic;

namespace Chroma
{
    /// <summary>
    /// Coroutine based on the IEnumerator system.
    /// 
    /// </summary>
    public class Coroutine
    {
        /// <summary>
        /// Whether the coroutine is active.
        /// </summary>
        public bool Active { get; private set; }

        /// <summary>
        /// Whether the coroutine has completed. Independent of <c cref="Active">Active</c>.
        /// </summary>
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
            if (!Active || Finished)
                return;

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

        /// <summary>
        /// Starts the coroutine.
        /// </summary>
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

        /// <summary>
        /// Pauses the coroutine.
        /// </summary>
        public void Pause()
        {
            Active = false;
        }

        /// <summary>
        /// Resumes the coroutine.
        /// </summary>
        public void Resume()
        {
            Active = true;
        }

        /// <summary>
        /// Restarts the coroutine.
        /// </summary>
        public void Restart()
        {
            Active = true;
            Finished = false;
            _waitTimer = 0;
            _ended = false;
            _enumerators.Clear();
            _enumerators.Push(_func());
        }

        /// <summary>
        /// Stops the coroutine.
        /// </summary>
        public void Stop()
        {
            Active = false;
            Finished = true;
            _waitTimer = 0;
            _enumerators.Clear();
            _ended = true;
        }

        /// <summary>
        /// Sets the function which the coroutine executes.
        /// </summary>
        /// <param name="function">Function returning an IEnumerator, yield return represent time values in seconds.</param>
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
