// Top-level build file where you can add configuration options common to all sub-projects/modules.
plugins {
    alias(libs.plugins.android.application) apply true
    alias(libs.plugins.kotlin.android) apply true
}
dependencies {
    implementation("com.google.android.exoplayer:exoplayer:2.19.1") // Core ExoPlayer library
    implementation("com.google.android.exoplayer:exoplayer-ui:2.18.6") // UI module for ExoPlayer
}


