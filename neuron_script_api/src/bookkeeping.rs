use crate::backend::ScriptBackend;

pub trait Plugin {
    fn terminate(&mut self);
}

#[repr(transparent)]
pub struct ScriptBackendRef(pub &'static mut dyn ScriptBackend);

///
/// This creates a static mutable variable called `PLUGIN` which contains an instance of Plugin
///
#[macro_export]
macro_rules! plugin_bookkeeping {
    ($plugin_type:ident) => {
        static mut PLUGIN: std::sync::Once<ManuallyDrop<$plugin_type>> = Once::new();
        #[unsafe(no_mangle)]
        pub unsafe extern "C" fn _plugin_init(backend: *mut dyn $crate::backend::ScriptBackend) -> *mut dyn $crate::bookkeeping::Plugin {
            PLUGIN.call_once(|| ManuallyDrop::new($plugin_type::new(ScriptBackendRef(unsafe { &mut *backend }))));
        }

        #[unsafe(no_mangle)]
        pub unsafe extern "C" fn _plugin_terminate() {
            PLUGIN.terminate();
        }
    }
}