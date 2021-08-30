# Audio Source

Binding for the C++ AudioComponent struct, found [here](structChroma_1_1AudioSource.html).




## Properties
| Name | Description |
| :----| :---|
| [`ID (readonly)`](#ID)     | Unique identifier for this component instance. |
| [`AllowsMultiple (readonly)`](#AllowsMultiple)     | Whether multiple instances of this component can exist for a single entity. |
| [`Name (readonly)`](#Name)     | String representation of the component type. |
| [`Event`](#Event)     | The FMOD audio event. |
| [`Mute`](#Mute)     | Whether the audio event is muted. |
| [`PlayOnInit`](#PlayOnInit)     | Whether the audio event should play immediately when the scene starts |



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
<h2 id="Event">Event</h2>

---
<code class="code_label">string</code>

The FMOD audio event which the AudioSource controls. Setting this event will change which event is loaded by the FMOD audio system. User is responsible for checking whether the event is valid.

<!-- START PROPERTY BLOCK -->
<h2 id="Mute">Mute</h2>

---
<code class="code_label">bool</code>

Boolean value representing whether the AudioSource will begin playing when the scene is initialized.

</br>