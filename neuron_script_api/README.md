# Neuron Script API

This crate contains the api for the scripting system.
Part of the scripting setup is storing a global non-owning reference to the script backend, which the interface to is
defined here.
However, the scripting api doesn't actually know how anything works, and relies on dynamically dispatched calls to the
actual runtime for that.