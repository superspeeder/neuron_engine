# Neuron Engine Runtime

This crate is the runtime program, which is responsible for actually running code.
This is the executable that is actually packaged as the application, and normally the executable that launches the
actual game is just an app which makes a shortcut to calling this with the right environment variables and cli args.

This crate is also responsible for implementing the scripting API (specifically the backend for the `neuron_script_api`
crate **not** the Lua scripting API, which is implemented by the main engine).

Theoretically, for performance an executable could be built based on this runtime which uses static dispatch for the
script api, but I currently have no plans to implement this (it'd be nice, but very much isn't a priority).