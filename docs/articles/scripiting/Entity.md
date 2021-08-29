# Entity

In the context of each script, the `entity` table is defined, providing the state and functions of the attached entity.



## Properties
`ID (readonly)`{:.lua}
> The entity's unique ID.

`Scene (readonly)`{:.lua}
> A reference to the scene in which the entity is stored.

`Name`{:.lua}
> The name of the entity.

## Methods
`bool Valid()`{:.lua}
> Returns whether the entity is Valid. An entity is valid if it contains a reference to a scene and has a valid ID registered by that scene.

`bool HasComponent(component_name)`{:.lua}
> Returns whether an entity has the specified component.

`bool HasAudioSource()`{:.lua}
> Returns whether an entity has an AudioSource component.

`bool HasBoxCollider2D()`{:.lua}
> Returns whether an entity has a BoxCollider2D component.

`bool HasCircleCollider2D()`{:.lua}
> Returns whether an entity has a CircleCollider2D component.

`bool HasSpriteRenderer()`{:.lua}
> Returns whether an entity has a SpriteRenderer component.

`Component AddComponent(component_name)`{:.lua}
> Adds a new component of the given name to the entity, and returns a reference to that component.

`AudioSource AddAudioSource()`{:.lua}
> Adds a new AudioSource to the entity and returns a reference to the new component.

`BoxCollider2D AddBoxCollider2D()`{:.lua}
> Adds a new BoxCollider2D to the entity and returns a reference to the new component.

`CircleCollider2D AddCircleCollider2D()`{:.lua}
> Adds a new CircleCollider2D to the entity and returns a reference to the new component.

`SpriteRenderer AddSpriteRenderer()`{:.lua}
> Adds a new SpriteRenderer to the entity and returns a reference to the new component.

`Component GetComponent(component_name)`{:.lua}
> Returns a reference to the entity's specified component.

`AudioSource GetAudioSource()`{:.lua}
> Returns a reference to the entity's AudioSource.

`BoxCollider2D GetBoxCollider2D()`{:.lua}
> Returns a reference to the entity's BoxCollider2D.

`CircleCollider2D GetCircleCollider2D()`{:.lua}
> Returns a reference to the entity's CircleCollider2D.

`SpriteRenderer GetSpriteRenderer()`{:.lua}
> Returns a reference to the entity's SpriteRenderer.