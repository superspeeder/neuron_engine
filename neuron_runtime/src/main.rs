use std::env;
use neuron_script_api::backend::ScriptBackend;
use neuron_script_api::bookkeeping::Plugin;

pub struct TestScriptBackend {

}

impl ScriptBackend for TestScriptBackend {

}

fn main() {
    env_logger::init();

    let arg1 = env::var("NEURON_RUNTIME_PLUGIN_DLL").unwrap();
    unsafe {
        let lib = libloading::Library::new(arg1).unwrap();
        let init_fn: libloading::Symbol<fn(*mut dyn ScriptBackend) -> *mut dyn Plugin> = lib.get(b"_plugin_init").unwrap();
        let terminate_fn: libloading::Symbol<fn()> = lib.get(b"_plugin_terminate").unwrap();
        
        let mut script_backend = Box::new(TestScriptBackend {});
        
        let plugin_ptr = init_fn(script_backend.as_mut() as *mut _);
        (&mut *plugin_ptr).test_callback();
        terminate_fn();
    }


    println!("Hello, world!");
}
