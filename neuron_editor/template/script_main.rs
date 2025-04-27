extern crate neuron_script_api;
use neuron_script_api::plugin_bookkeeping;
use neuron_script_api::bookkeeping::Plugin as PluginInterface;
use neuron_script_api::bookkeeping::ScriptBackendRef;

pub struct Plugin {
    _backend: ScriptBackendRef,
}

impl Plugin {
    pub fn new(backend: ScriptBackendRef) -> Plugin {
        Plugin { _backend: backend }
    }
}

impl PluginInterface for Plugin {
    fn test_callback(&mut self) {
    }
}

plugin_bookkeeping!(Plugin);
