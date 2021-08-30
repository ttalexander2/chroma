# Component

Binding for the c++ abstract struct, found [here](structChroma_1_1Component.html).
As Lua does not support objects, this will outline the basic functionality of any component bound to Lua.



## Properties
| Name | Description |
| :----| :---|
| [`ID (readonly)`](#ID)     | Unique identifier for this component instance. |
| [`AllowsMultiple (readonly)`](#AllowsMultiple)     | Whether multiple instances of this component can exist for a single entity. |
| [`Name (readonly)`](#Name)     | String representation of the component type. |



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

</br>