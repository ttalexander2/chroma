# Chroma API
Welcome to the Chroma API Documentation. The Chroma Engine features a full C# API system, which may be referenced here.

```csharp
using System.Collections;
using Chroma;


namespace MyGame
{
    //All game objects by default extend the Entity class.
    //The entity class provides methods for accessing object data, components, and behavior.
    public class MyEnemy : Entity
    {
        //Public fields will be visible within the editor.
        public float Speed = 10.0f;

        //Private references
        private Transform _transform;
        private SpriteRenderer _spriteRenderer;

        //This function will be called at the beginning of the Scene.
        public override void Init()
        {
            //This will write a string to the console.
            Log.Info("I am a flea");

            //This retrieves a reference to this Entity's Transform Component.
            //It is recommended to retrieve components during initialization instead of runtime.
            _transform = GetComponent<Transform>();
        }

        //This function will run once per frame, at a variable rate.
        public override void Update()
        {
            //Here we modify the position to move the enemy to the right.
            //Note we use Time.DeltaF here to take into account the time since the last frame occurred.
            _transform.Position = _transform.Position + new Vector2(1, 0) * Speed * Time.DeltaF;

            Log.Info("My position is {0}", _transform.Position);
        }

    }
}
```
