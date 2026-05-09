#pragma once
#include <sol/sol.hpp>

namespace cme {
    enum class KeyCode {
        // Letras
        A = 65, B, C, D, E, F, G, H, I, J, K, L, M,
        N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

        // Números fila superior
        Num0 = 48, Num1, Num2, Num3, Num4,
        Num5, Num6, Num7, Num8, Num9,

        // Función
        F1 = 290, F2, F3, F4, F5, F6,
        F7, F8, F9, F10, F11, F12,

        // Especiales
        Space = 32,
        Enter = 257,
        Escape = 256,
        Tab = 258,
        Backspace = 259,
        Delete = 261,
        Insert = 260,

        // Flechas
        Right = 262,
        Left = 263,
        Down = 264,
        Up = 265,

        // Modificadores
        LeftShift = 340,
        RightShift = 344,
        LeftCtrl = 341,
        RightCtrl = 345,
        LeftAlt = 342,
        RightAlt = 346,

        // Numpad
        KP0 = 320, KP1, KP2, KP3, KP4,
        KP5, KP6, KP7, KP8, KP9,
        KPDecimal = 330,
        KPEnter = 335,
        KPAdd = 334,
        KPSubtract = 333,
        KPMultiply = 332,
        KPDivide = 331,

        // Navegación
        PageUp = 266,
        PageDown = 267,
        Home = 268,
        End = 269,

        Unknown = -1
    };

    inline void registerKeyCode(sol::state& lua) {
        lua.new_enum<KeyCode>("Key", {
            // Letras
            { "A", KeyCode::A }, { "B", KeyCode::B }, { "C", KeyCode::C },
            { "D", KeyCode::D }, { "E", KeyCode::E }, { "F", KeyCode::F },
            { "G", KeyCode::G }, { "H", KeyCode::H }, { "I", KeyCode::I },
            { "J", KeyCode::J }, { "K", KeyCode::K }, { "L", KeyCode::L },
            { "M", KeyCode::M }, { "N", KeyCode::N }, { "O", KeyCode::O },
            { "P", KeyCode::P }, { "Q", KeyCode::Q }, { "R", KeyCode::R },
            { "S", KeyCode::S }, { "T", KeyCode::T }, { "U", KeyCode::U },
            { "V", KeyCode::V }, { "W", KeyCode::W }, { "X", KeyCode::X },
            { "Y", KeyCode::Y }, { "Z", KeyCode::Z },

            // Números
            { "Num0", KeyCode::Num0 }, { "Num1", KeyCode::Num1 },
            { "Num2", KeyCode::Num2 }, { "Num3", KeyCode::Num3 },
            { "Num4", KeyCode::Num4 }, { "Num5", KeyCode::Num5 },
            { "Num6", KeyCode::Num6 }, { "Num7", KeyCode::Num7 },
            { "Num8", KeyCode::Num8 }, { "Num9", KeyCode::Num9 },

            // Función
            { "F1",  KeyCode::F1  }, { "F2",  KeyCode::F2  },
            { "F3",  KeyCode::F3  }, { "F4",  KeyCode::F4  },
            { "F5",  KeyCode::F5  }, { "F6",  KeyCode::F6  },
            { "F7",  KeyCode::F7  }, { "F8",  KeyCode::F8  },
            { "F9",  KeyCode::F9  }, { "F10", KeyCode::F10 },
            { "F11", KeyCode::F11 }, { "F12", KeyCode::F12 },

            // Especiales
            { "Space",     KeyCode::Space     },
            { "Enter",     KeyCode::Enter     },
            { "Escape",    KeyCode::Escape    },
            { "Tab",       KeyCode::Tab       },
            { "Backspace", KeyCode::Backspace },
            { "Delete",    KeyCode::Delete    },
            { "Insert",    KeyCode::Insert    },

            // Flechas
            { "Right", KeyCode::Right },
            { "Left",  KeyCode::Left  },
            { "Down",  KeyCode::Down  },
            { "Up",    KeyCode::Up    },

            // Modificadores
            { "LeftShift",  KeyCode::LeftShift  },
            { "RightShift", KeyCode::RightShift },
            { "LeftCtrl",   KeyCode::LeftCtrl   },
            { "RightCtrl",  KeyCode::RightCtrl  },
            { "LeftAlt",    KeyCode::LeftAlt    },
            { "RightAlt",   KeyCode::RightAlt   },

            // Numpad
            { "KP0", KeyCode::KP0 }, { "KP1", KeyCode::KP1 },
            { "KP2", KeyCode::KP2 }, { "KP3", KeyCode::KP3 },
            { "KP4", KeyCode::KP4 }, { "KP5", KeyCode::KP5 },
            { "KP6", KeyCode::KP6 }, { "KP7", KeyCode::KP7 },
            { "KP8", KeyCode::KP8 }, { "KP9", KeyCode::KP9 },
            { "KPDecimal",  KeyCode::KPDecimal  },
            { "KPEnter",    KeyCode::KPEnter    },
            { "KPAdd",      KeyCode::KPAdd      },
            { "KPSubtract", KeyCode::KPSubtract },
            { "KPMultiply", KeyCode::KPMultiply },
            { "KPDivide",   KeyCode::KPDivide   },

            // Navegación
            { "PageUp",   KeyCode::PageUp   },
            { "PageDown", KeyCode::PageDown },
            { "Home",     KeyCode::Home     },
            { "End",      KeyCode::End      },

            { "Unknown", KeyCode::Unknown }
            });
        }
}