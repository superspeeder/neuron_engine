use crate::project::buildsystem::BuildsystemConfiguration;

#[derive(Clone, serde::Serialize, serde::Deserialize)]
pub struct ProjectManifest {
    pub name: String,
    pub build: BuildsystemConfiguration,
}
