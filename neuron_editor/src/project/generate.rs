use std::io;
use std::path::Path;
use crate::project::buildsystem::generate_build_project;
use crate::project::generate::GenerateProjectError::{TargetFolderNotEmpty, TargetPathIsNotAFolder};
use crate::project::manifest::ProjectManifest;

#[derive(thiserror::Error, Debug)]
pub enum GenerateProjectError {
    #[error("Target folder is not empty")]
    TargetFolderNotEmpty,

    #[error("Target path is not a folder")]
    TargetPathIsNotAFolder,

    #[error(transparent)]
    TomlError(#[from] toml::ser::Error),

    #[error(transparent)]
    IoError(#[from] io::Error),
}

///
/// Generate a project skeleton in the target directory
///
/// The standard skeleton is
///
/// <project-dir>/
/// |  scripts/
/// |  |  script_main.rs
/// |  assets/
/// |  .build/
/// |  |  Cargo.toml
/// |  project.toml
/// |  .gitignore
///
///
///
/// NOTE: scripts contains the rust scripts which get built into a cdylib and not lua scripts. We integrate (not yet but planned) LuaJIT and can load lua scripts from assets (these can change at runtime, rust "scripts" cannot be, but are likely faster)
/// script_main.rs contains the basic skeleton required to properly setup exporting the code for the runtime.
///
/// the .build folder contains the build system files and some other important data files (certain project files which have information that doesn't fit into the project files. For example, the cached prebuilt shaders will likely get stored here. `.build` should not be included in VCS.
///
pub fn generate_project_skeleton(directory: impl AsRef<Path>, manifest: ProjectManifest) -> Result<(), GenerateProjectError> {
    let path = directory.as_ref();
    if !path.exists() {
        std::fs::create_dir_all(path)?;
    } else {
        if !path.is_dir() {
            return Err(TargetPathIsNotAFolder);
        } else if std::fs::read_dir(path)?.next().is_some() {
            return Err(TargetFolderNotEmpty);
        }
    }

    // write manifest
    let manifest_str = toml::to_string_pretty(&manifest)?;
    std::fs::write(path.join("project.toml"), manifest_str)?;
    std::fs::create_dir(path.join("scripts"))?;
    std::fs::create_dir(path.join("assets"))?;
    std::fs::write(path.join("scripts/script_main.rs"), generate_script_main(&manifest))?;
    std::fs::write(path.join(".gitignore"), include_str!("../../template/template.gitignore"))?;
    generate_build_project(path, &manifest.build)?;


    Ok(())
}

fn generate_script_main(manifest: &ProjectManifest) -> String {
    format!(include_str!("../../template/script_main.rs"))
}
