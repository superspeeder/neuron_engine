use crate::backend::ScriptBackend;

pub trait Plugin {
    fn test_callback(&mut self);
}

#[repr(transparent)]
pub struct ScriptBackendRef(pub &'static mut dyn ScriptBackend);

///
/// This creates a static mutable variable called `PLUGIN` which contains an instance of Plugin
/// Also creates the functions `get_plugin_mut()` and `get_plugin()` which are not marked as unsafe *but* are only safe because all plugin code is called in controlled contexts. Don't call this from anything that might escape those contexts.
///
#[macro_export]
macro_rules! plugin_bookkeeping {
    ($plugin_type:ident) => {
        static mut PLUGIN: std::sync::OnceLock<*mut $plugin_type> = std::sync::OnceLock::new();

        #[unsafe(no_mangle)]
        pub unsafe extern "stdcall" fn _plugin_init(
            backend: *mut dyn $crate::backend::ScriptBackend,
        ) -> *mut dyn $crate::bookkeeping::Plugin {
            use std::ops::DerefMut;
            let backend_ref = $crate::bookkeeping::ScriptBackendRef(&mut *backend);
            let plugin_box = std::boxed::Box::new($plugin_type::new(backend_ref));
            let _ = PLUGIN.set(std::boxed::Box::leak(plugin_box));
            let ptrref = PLUGIN.get_or_init(|| unreachable!())
            let ptr = Clone::clone(ptrref) as *mut dyn $crate::bookkeeping::Plugin;
            ptr
        }

        #[unsafe(no_mangle)]
        pub unsafe extern "stdcall" fn _plugin_terminate() {
            if let Some(&plug) = PLUGIN.get() {
                std::mem::drop(std::boxed::Box::from_raw(plug as *mut $plugin_type)); // rebox the memory and immediately drop it
            };
        }
    };
}
