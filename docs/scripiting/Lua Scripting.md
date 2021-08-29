# Lua Scripting

The chroma engine contains bindings to support behavior in lua. A set of custom functions and environments are provided to control and interact with the Entity Component System provided by the engine.

Each script can be attached to an entity, and is provided with that entities' context. The script will be executed in the context of that entity, and can be attached to multiple entities.