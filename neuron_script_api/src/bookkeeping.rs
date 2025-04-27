use crate::backend::ScriptBackend;

pub trait Plugin {
}

#[repr(transparent)]
pub struct ScriptBackendRef(pub &'static mut dyn ScriptBackend);

///
/// This creates a static mutable variable called `PLUGIN` which contains an instance of Plugin
///
#[macro_export]
macro_rules! plugin_bookkeeping {
    ($plugin_type:ident) => {
        static mut PLUGIN: std::sync::OnceLock<std::mem::ManuallyDrop<$plugin_type>> = std::sync::OnceLock::new();
        #[unsafe(no_mangle)]
        pub unsafe extern "C" fn _plugin_init(backend: *mut dyn $crate::backend::ScriptBackend) -> *mut dyn $crate::bookkeeping::Plugin {
            use std::ops::DerefMut;
            PLUGIN.get_mut_or_init(||std::mem::ManuallyDrop::new($plugin_type::new($crate::bookkeeping::ScriptBackendRef(unsafe { &mut *backend })))).deref_mut() as *mut $plugin_type as *mut dyn $crate::bookkeeping::Plugin
        }

        #[unsafe(no_mangle)]
        pub unsafe extern "C" fn _plugin_terminate() {
            if let Some(plug) = PLUGIN.get_mut() { std::mem::ManuallyDrop::drop(plug) };
        }
    }
}