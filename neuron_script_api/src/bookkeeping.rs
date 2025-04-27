use crate::backend::ScriptBackend;
use std::mem::ManuallyDrop;
use std::ops::Deref;
use std::sync::Arc;
use parking_lot::RwLock;

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
        static mut PLUGIN: std::sync::OnceLock<*mut $plugin_type> = std::sync::OnceLock::new();
        #[unsafe(no_mangle)]
        pub unsafe extern "C" fn _plugin_init(backend: *mut dyn $crate::backend::ScriptBackend) -> *mut dyn $crate::bookkeeping::Plugin {
            use std::ops::DerefMut;
            *PLUGIN.get_or_init(|| std::boxed::Box::leak(std::boxed::Box::new($plugin_type::new($crate::bookkeeping::ScriptBackendRef(backend)))))
                as *mut dyn $crate::bookkeeping::Plugin
        }

        #[unsafe(no_mangle)]
        pub unsafe extern "C" fn _plugin_terminate() {
            if let Some(&plug) = PLUGIN.get() {
                std::mem::drop(std::boxed::Box::from_raw(plug as *mut $plugin_type)); // rebox the memory and immediately drop it
            };
        }
    }
}