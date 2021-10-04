using System;
using System.Collections;
using System.Collections.Generic;

namespace Chroma
{
    public class Coroutine
    {

        public bool Finished { get; private set; }

        private Stack<IEnumerator> _enumerators = new Stack<IEnumerator>();

        private float _waitTimer = 0;
        private bool _ended = false;

        internal Coroutine()
        {
        }

        internal Coroutine(IEnumerator function)
        {
            _enumerators.Push(function);
        }

        public void Update()
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

        public void Cancel()
        {
            Finished = true;
            _waitTimer = 0;
            _enumerators.Clear();

            _ended = true;
        }

        public void Set(IEnumerator functionCall)
        {
            Finished = false;
            _waitTimer = 0;
            _enumerators.Clear();
            _enumerators.Push(functionCall);

            _ended = false;
        }

        public void Add(IEnumerator functionCall)
        {
            Finished = false;
            _enumerators.Push(functionCall);
        }
    }
}
