# Entity

In the context of each script, the `entity` table is defined, providing the state and functions of the attached entity.



## Properties
`ID (readonly)`
The entity's unique ID.

`Scene (readonly)`
A reference to the scene in which the entity is stored.

`Name`
The name of the entity.

## Methods
`bool Valid()`
Returns whether the entity is Valid. An entity is valid if it contains a reference to a scene and has a valid ID registered by that scene.

`bool HasComponent(component_name)`
Returns whether an entity has 


