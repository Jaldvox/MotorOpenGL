cmake_minimum_required(VERSION 3.20)
project(CapiEngine VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Añadimos los subproyectos
add_subdirectory(EngineCore)
add_subdirectory(EngineEditor)
add_subdirectory(Runtime)

# --- Configuración de Versión y Nombre ---
set(CPACK_PACKAGE_NAME "Capi Engine")
set(CPACK_PACKAGE_VENDOR "CapitanManzana")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Capi Engine - Versión Alpha")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-alpha")

# --- Reglas de Instalación ---

# 1. Instalación de los Ejecutables (Editor y Runtime) y DLLs
# El destino "." se refiere a la carpeta raíz que el usuario elija (ej. C:/Program Files/CapiEngine)
install(TARGETS EngineEditor EngineRuntime
        RUNTIME DESTINATION "."
        LIBRARY DESTINATION "."
        ARCHIVE DESTINATION ".")

# 2. Instalación de DLLs de terceros (como Assimp, Glad, etc.)
# Si tienes una variable con las DLLs, o puedes usar un glob (ajusta la ruta a tu carpeta de binarios)
file(GLOB THIRD_PARTY_DLLS "${CMAKE_BINARY_DIR}/*.dll")
install(FILES ${THIRD_PARTY_DLLS} DESTINATION ".")

# 3. Carpeta de Recursos (Shaders)
# Esto copiará tu carpeta de shaders dentro de una subcarpeta 'resources' en la instalación
install(DIRECTORY "${CMAKE_SOURCE_DIR}/out/build/x64-Debug/EngineEditor/resources" 
        DESTINATION "./")

# --- Configuración específica del instalador (NSIS) ---
set(CPACK_GENERATOR "NSIS")
set(CPACK_NSIS_PACKAGE_NAME "Capi Engine Alpha")
set(CPACK_NSIS_DISPLAY_NAME "Capi Engine v${CPACK_PACKAGE_VERSION}")

# Permite que el usuario elija la carpeta de instalación (Program Files por defecto)
set(CPACK_NSIS_INSTALL_ROOT "C:/Program Files")
set(CPACK_NSIS_INSTALLED_ICON_NAME "EngineEditor.exe") # Icono del desinstalador

include(CPack)