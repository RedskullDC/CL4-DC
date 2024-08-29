// swift-tools-version: 5.10
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "example",
    targets: [

        .executableTarget(
            name: "example",

            // example executable requires "CL4" target as its dependency.
            // It's a systemLibrary target defined below.
            dependencies: ["CL4"],
            path: "Sources"
        ),

        // systemLibrary is a special type of build target that wraps a system library
        // in a target that other targets can require as their dependency.
        .systemLibrary(
            name: "CL4",
            pkgConfig: "libcl4",
            providers: [
                .brew(["libcl4"]),
                .apt(["libcl4"])
            ]
        )

    ]
)
