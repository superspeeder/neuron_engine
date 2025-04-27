extern crate neuron_script_api;
use neuron_script_api::plugin_bookkeeping;
use neuron_script_api::bookkeeping::Plugin as PluginInterface;
use neuron_script_api::bookkeeping::ScriptBackendRef;
use neuron_script_api::log::info;

pub struct Plugin {
    backend: ScriptBackendRef,
}

impl Plugin {
    pub fn new(backend: ScriptBackendRef) -> Plugin {
        info!("Hello plugin!");
        Plugin { backend }
    }
}

impl PluginInterface for Plugin {
    fn terminate(&mut self) {
        info!("Goodbye plugin!");
    }
}

plugin_bookkeeping!(Plugin);
