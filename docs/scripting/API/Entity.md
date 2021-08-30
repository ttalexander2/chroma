# Entity

In the context of each script, the `entity` table is defined, providing the state and functions of the attached entity.

This table is equivalent to the C++ class found [here](classChroma_1_1Entity.html).



## Properties
| Name | Description |
| :----| :---|
| [`ID (readonly)`](#ID)          | The entity's unique ID. |
| [`Scene (readonly)`](#Scene)    | the scene in which the entity is stored. |
| [`Name`](#Name)                 | The name of the entity. |


## Methods


| Name | Description |
| :-----------|:-----|
| [`Valid`](#Valid)                                     | Returns whether the entity is Valid. |
| [`HasComponent`](#HasComponent)                              | Returns whether an entity has the specified component. |
| [`HasAudioSource`](#HasAudioSource)                            | Returns whether an entity has an AudioSource component. |
| [`HasBoxCollider2D`](#HasBoxCollider2D)                          | Returns whether an entity has a BoxCollider2D component. |
| [`HasCircleCollider2D`](#HasCircleCollider2D)                       |  Returns whether an entity has a CircleCollider2D component. |
| [`HasSpriteRenderer`](#HasSpriteRenderer)                         | Returns whether an entity has a SpriteRenderer component. |
| [`AddComponent`](#AddComponent)                              | Adds a new component of the given name to the entity. |
| [`AddAudioSource`](#AddAudioSource)                            | Adds a new AudioSource to the entity and returns the new component. |
| [`AddBoxCollider2D`](#AddBoxCollider2D)                          | Adds a new BoxCollider2D to the entity and returns the new component. |
| [`AddCircleCollider2D`](#AddCircleCollider2D)                       | Adds a new CircleCollider2D to the entity and returns the new component. |
| [`AddSpriteRenderer`](#AddSpriteRenderer)                         | Adds a new SpriteRenderer to the entity and returns the new component. |
| [`GetComponent`](#GetComponent)                              | Returns the entity's specified component. |
| [`GetAudioSource`](#GetAudioSource)                            | Returns the entity's AudioSource. |
| [`GetBoxCollider2D`](#GetBoxCollider2D)                          | Returns the entity's BoxCollider2D. |
| [`GetCircleCollider2D`](#GetCircleCollider2D)                       | Returns the entity's CircleCollider2D. |
| [`GetSpriteRenderer`](#GetSpriteRenderer)                         | Returns the entity's SpriteRenderer. |


<h1> Property Detail </h1>
<!-- ID -->
<h2 id="ID">ID</h2>

---
<code class="code_label">readonly int</code>

The ID of the entity, as provided by the [Scene](#Scene) registry.


<!-- SCENE -->
<h2 id="Scene">Scene</h2>

---
<code class="code_label">readonly Scene</code>

Provides a reference to the [Scene](#Scene) storing the entity.

<!-- SCENE -->
<h2 id="Name">Name</h2>

---
<code class="code_label">string</code>

Name of the entitiy. This property is readable and writable. This is the name that will show up in the Hierarchy/Inspector of the editor.

<br/>

<h1> Method Detail </h1>

<h2 id="Valid">Valid</h2>

---
`bool Valid()`

Returns whether an entity is valid. An entity is considered valid if the ID is registered by the scene, and the reference to the scene is not null.

Usage:
```lua
local res = entity:Valid()
```
<br /> 

<h2 id="HasComponent">HasComponent</h2>

---

`bool HasComponent(string component)`

Returns whether an entity has the specified component.

### Parameters:
`string` The name of the component (must match the components' signature).

### Usage:
```lua
local res = entity:HasComponent("CircleCollider2D")
```
<br /> 

<h2 id="HasAudioSource">HasAudioSource</h2>

---
`bool HasAudioSource()`

Returns whether an entity has an [AudioSource](#) component.

### Returns:
<code class="code_label">bool</code> Whether the entity has an AudioSource.

### Usage:
```lua
local res = entity:HasAudioSource()
```
<br /> 

<h2 id="HasBoxCollider2D">HasBoxCollider2D</h2>

---
`bool HasBoxCollider2D()`

Returns whether an entity has a [BoxCollider2D](#) component.

### Returns:
<code class="code_label">bool</code> Whether the entity has a BoxCollider2D.

### Usage:
```lua
local res = entity:HasBoxCollider2D()
```
<br /> 

<h2 id="HasCircleCollider2D">HasCircleCollider2D</h2>

---
`bool HasCircleCollider2D()`

Returns whether an entity has a [CircleCollider2D](#) component.

### Returns:
<code class="code_label">bool</code>  Whether the entity has a CircleCollider2D.

### Usage:
```lua
local res = entity:HasCircleCollider2D()
```
<br /> 

<h2 id="HasSpriteRenderer">HasSpriteRenderer</h2>

---
`bool HasSpriteRenderer()`

Returns whether an entity has a [SpriteRenderer](#) component.

### Returns:
<code class="code_label">bool</code>  Whether the entity has a SpriteRenderer.

### Usage:
```lua
local res = entity:HasSpriteRenderer()
```
<br /> 

<h2 id="AddComponent">AddComponent</h2>

---
`Component AddComponent(string component)`

Adds a new [Component](#) to the entity of the specifying type.

### Returns:
<code class="code_label">Component</code>  A reference to the new component.
### Parameters:
`string` The name of the component (must match the components' signature).

### Usage:
```lua
local comp = entity:AddComponent("SpriteRenderer")
```
<br /> 

<h2 id="AddAudioSource">AddAudioSource</h2>

---
`AudioSource AddAudioSource()`

Adds a new empty [AudioSource](#) to the entity.

### Returns:
<code class="code_label">AudioSource</code>  A reference to the new AudioSource.

### Usage:
```lua
local comp = entity:AddAudioSource()
```
<br /> 

<h2 id="AddBoxCollider2D">AddBoxCollider2D</h2>

---
`BoxCollider2D AddBoxCollider2D()`

Adds a new empty [BoxCollider2D](#) to the entity.

### Returns:
<code class="code_label">BoxCollider2D</code> A reference to the new BoxCollider2D.

### Usage:
```lua
local box_collider = entity:AddBoxCollider2D()
```
<br /> 

<h2 id="AddCircleCollider2D">AddCircleCollider2D</h2>

---
`BoxCollider2D AddCircleCollider2D()`

Adds a new empty [CircleCollider2D](#) to the entity.

### Returns:
<code class="code_label">CircleCollider2D</code> A reference to the new CircleCollider2D.

### Usage:
```lua
local circle_collider = entity:AddCircleCollider2D()
```
<br /> 

<h2 id="AddSpriteRenderer">AddSpriteRenderer</h2>

---
`SpriteRenderer AddSpriteRenderer()`

Adds a new empty [SpriteRenderer](#) to the entity.

### Returns:
<code class="code_label">SpriteRenderer</code> A reference to the new SpriteRenderer.

### Usage:
```lua
local sprite = entity:AddSpriteRenderer()
```
<br /> 

<h2 id="GetComponent">GetComponent</h2>

---
`Component GetComponent(string component)`

Gets an entity's [Component](#) of the specifying type.

### Returns:
<code class="code_label">Component</code> A reference to the component. Can return `nil` if the component does not exist.
### Parameters:
<code class="code_label">string</code> The name of the component (must match the components' signature).

### Usage:
```lua
local comp = entity:GetComponent("SpriteRenderer")
```
<br /> 

<h2 id="GetAudioSource">GetAudioSource</h2>

---
`AudioSource GetAudioSource()`

Gets a reference to the entity's [AudioSource](#).
This function will construct a new AudioSource if one does not exist for the
given entity.

### Returns:
<code class="code_label">AudioSource</code> A reference to the AudioSource component.

### Usage:
```lua
local audio_source = entity:GetAudioSource()
```
<br /> 

<h2 id="GetBoxCollider2D">GetBoxCollider2D</h2>

---
`BoxCollider2D GetBoxCollider2D()`

Gets a reference to the entity's [BoxCollider2D](#).
This function will construct a new BoxCollider2D if one does not exist for the
given entity.

### Returns:
<code class="code_label">BoxCollider2D</code> A reference to the BoxCollider2D component.

### Usage:
```lua
local box_collider = entity:GetBoxCollider2D()
```
<br /> 

<h2 id="GetCircleCollider2D">GetCircleCollider2D</h2>

---
`CircleCollider2D GetCircleCollider2D()`

Gets a reference to the entity's [CircleCollider2D](#).
This function will construct a new CircleCollider2D if one does not exist for the
given entity.

### Returns:
<code class="code_label">CircleCollider2D</code> A reference to the CircleCollider2D component.

### Usage:
```lua
local circle_collider = entity:GetCircleCollider2D()
```
<br /> 

<h2 id="GetSpriteRenderer">GetSpriteRenderer</h2>

---
`SpriteRenderer GetSpriteRenderer()`

Gets a reference to the entity's [SpriteRenderer](#).
This function will construct a new SpriteRenderer if one does not exist for the
given entity.

### Returns:
<code class="code_label">SpriteRenderer</code> A reference to the SpriteRenderer component.

### Usage:
```lua
local sprite = entity:GetSpriteRenderer()
```
