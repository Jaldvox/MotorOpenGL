# CapiEngine Runtime

## Descripción
El Runtime es el ejecutable que carga y reproduce escenas guardadas en formato JSON.

## Requisitos

1. **Carpeta `build/`** en la raíz del proyecto
2. **Runtime.exe** compilado
3. **Runtime.json** en la misma carpeta que Runtime.exe
4. **Carpeta `assets/`** con shaders, texturas y escenas

## Estructura de Carpetas

```
Proyecto/
├── build/                    # Carpeta de compilación
│   ├── Runtime.exe          # Ejecutable del runtime
│   ├── Runtime.json         # Configuración (REQUERIDO)
│   └── assets/              # Copia de assets (simbólico o real)
│       ├── scenes/
│       ├── shaders/
│       ├── textures/
│       └── images/
├── EngineCore/
├── EngineEditor/
└── assets/                  # Assets originales
```

## Uso

### Opción 1: Desde el archivo de configuración (Recomendado)
```bash
cd build
./Runtime.exe
```
Busca `Runtime.json` y carga la escena especificada en `startScene`.

### Opción 2: Desde línea de comandos
```bash
cd build
./Runtime.exe assets/scenes/minecraft.json
```

## Archivos de Configuración

### Runtime.json
```json
{
    "startScene": "assets/scenes/minecraft.json"
}
```

**Ubicación:** `build/Runtime.json`  
**Contenido:** Ruta relativa desde la carpeta build hacia el JSON de la escena

## Solución de Problemas

### Error: "Archivo de configuración no encontrado"
- Verificar que existe `build/Runtime.json`
- Crear el archivo con la estructura mostrada arriba

### Error: "Escena no encontrada"
- Verificar la ruta en `startScene`
- Asegurarse que la ruta es relativa desde la carpeta build
- Ejemplo correcto: `"assets/scenes/minecraft.json"`

### Error: "El MeshRenderer no tiene Transform"
- Abrir el archivo JSON de la escena
- Verificar que todas las entidades con MeshRenderer tengan Transform
- Ejemplo estructura de entidad:
```json
{
  "name": "Cube",
  "components": [
    {
      "type": "Transform",
      "data": { "position": [0, 0, 0], ... }
    },
    {
      "type": "MeshRenderer",
      "data": { "mesh": "Cube", ... }
    }
  ]
}
```

## Controles

- **W/A/S/D** - Mover cámara
- **Shift** - Alternar modos de cámara
- **Mouse** - Rotar vista

## Recomendaciones

1. Usar ruta absoluta en compilación de Assets:
   ```cmake
   add_custom_command(TARGET Runtime POST_BUILD
       COMMAND ${CMAKE_COMMAND} -E copy_directory
       ${CMAKE_SOURCE_DIR}/assets
       $<CONFIG>/build/assets
   )
   ```

2. Crear symlink en build si es posible:
   ```bash
   cd build
   ln -s ../assets assets  # Linux/Mac
   mklink /D assets ../assets  # Windows
   ```

## Notas para Desarrolladores

- El Runtime carga recursos automáticamente desde la carpeta `assets/`
- Soporta cargar shaders, texturas y scripts Lua
- Usa el archivo de escena para instanciar entidades
- Los componentes se inicializan DESPUÉS de cargar todos
