# BoxCollider2D

Binding for the C++ BoxCollider2D struct, found [here](structChroma_1_1BoxCollider2D.html).

The box collider position is given by the [Transform](md_scripting_API_Transform.html) combined with the [Offset](#Offset).



## Properties
| Name | Description |
| :----| :---|
| [`ID (readonly)`](#ID)     | Unique identifier for this component instance. |
| [`AllowsMultiple (readonly)`](#AllowsMultiple)     | Whether multiple instances of this component can exist for a single entity. |
| [`Name (readonly)`](#Name)     | String representation of the component type. |
| [`Bounds`](#Bounds)     | Bounds of the collider (aka the size). |
| [`Offset`](#Offset)     | The offset to the position of the collider. |



# Property Detail
<!-- ID -->
<h2 id="ID">ID</h2>

---
<code class="code_label">readonly int</code>

An unique identifier for the component instance. This is used to test components for equality, or to sort in a list.

<!-- START PROPERTY BLOCK -->
<h2 id="AllowsMultiple">AllowsMultiple</h2>

---
<code class="code_label">readonly bool</code>

A value, representing whether the component type allows for multiple components of this type to be assigned to a single entity.

<!-- START PROPERTY BLOCK -->
<h2 id="Name">Name</h2>

---
<code class="code_label">readonly string</code>

The type of the component, represented as a string. This string is used for reflection purposes.

<!-- START PROPERTY BLOCK -->
<h2 id="Bounds">Bounds</h2>

---
<code class="code_label">[Vec2](md_scripting_API_Vec2.html)</code>

Bounds of the collider. The vector contains a width and a height. The [Transform](md_scripting_API_Transform.html) and the [Offset](#Offset) is used for the collider's position.

<!-- START PROPERTY BLOCK -->
<h2 id="Offset">Offset</h2>

---
<code class="code_label">[Vec2](md_scripting_API_Vec2.html)</code>

Positional offset of the collider. The vector contains an x, y offset which is combined with the [Transform](md_scripting_API_Transform.html).