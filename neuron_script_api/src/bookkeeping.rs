use std::cell::RefCell;
use crate::backend::ScriptBackend;

pub trait Plugin {
    fn test_callback(&mut self);
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
        static mut PLUGIN: std::sync::OnceLock<std::boxed::Box<std::cell::RefCell<dyn $crate::bookkeeping::Plugin>>> = std::sync::OnceLock::new();

        #[unsafe(no_mangle)]
        pub unsafe extern "stdcall" fn _plugin_init(
            backend: *mut dyn $crate::backend::ScriptBackend,
        ) -> *mut dyn $crate::bookkeeping::Plugin {
            use std::ops::DerefMut;
            let backend_ref = $crate::bookkeeping::ScriptBackendRef(&mut *backend);
            let mut plugin_box = std::boxed::Box::new(std::cell::RefCell::new($plugin_type::new(backend_ref)));
            let ptr = std::boxed::Box::as_mut_ptr();
            let _ = PLUGIN.set(plugin_box);
            ptr
        }

        #[unsafe(no_mangle)]
        pub unsafe extern "stdcall" fn _plugin_terminate() {
            if let Some(plug) = PLUGIN.get() {
                plug.borrow_mut().terminate();
            };
        }
    };
}
